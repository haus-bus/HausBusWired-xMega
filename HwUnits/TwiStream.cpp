/*
 * TwiStream.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "TwiStream.h"
#include "HomeAutomationInterface.h"
#include <Peripherals/ResetSystem.h>
#include <util/delay.h>

const uint8_t TwiStream::debugLevel( DEBUG_LEVEL_OFF );

void TwiStream::Response::setConfiguration( uint8_t value )
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().configuration) );
  setResponse( CONFIGURATION );
  getParameter().configuration = value;
}

void TwiStream::Response::setBusTiming( BusTiming& timing )
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().busTiming) );
  setResponse( BUS_TIMING );
  getParameter().busTiming = timing;
}

void TwiStream::Response::setConnectedDevices( DeviceArray* deviceArray )
{
  uint8_t size = 0;
  if ( deviceArray )
  {
    size = deviceArray->size();
  }
  controlFrame.setDataLength(
      sizeof(getResponse())
          + sizeof(getParameter().connectedDevices.deviceIds[0]) * size );
  setResponse( CONNECTED_DEVICES );
  if ( deviceArray )
  {
    for ( uint8_t i = 0; i < size; i++ )
    {
      uint16_t* ids = deviceArray->begin();
      getParameter().connectedDevices.deviceIds[i] = ids[i];
    }
  }
}

void TwiStream::Response::setBusPerformance( uint16_t duration, DeviceArray* failedDevices )
{
  uint8_t size = 0;
  if ( failedDevices )
  {
    size = failedDevices->size();
  }
  controlFrame.setDataLength(
      sizeof(getResponse())
          + sizeof(getParameter().busPerformance.duration)
          + sizeof(getParameter().busPerformance.failedDeviceIds[0]) * size );
  setResponse( BUS_PERFORMANCE );
  getParameter().busPerformance.duration = duration;
  if ( failedDevices )
  {
    for ( uint8_t i = 0; i < size; i++ )
    {
      getParameter().busPerformance.failedDeviceIds[i] = failedDevices->getElement(i);
    }
  }
}

#ifdef _DEBUG_
uint8_t TwiStream::getTraceInfo()
{
  return ((uint8_t*) &twi)[4];    // M.STATUS
  //return ((uint8_t*)&twi)[10];    // S.STATUS
}
#endif

void TwiStream::handleMasterStatus()
{
  timeout = Timestamp();
  bool finish = (masterDescriptor.pData == 0);

  if ( !finish && masterDescriptor.bytesRemaining )
  {
    masterDescriptor.bytesTransferred++;
    masterDescriptor.bytesRemaining--;
  }
  finish = (masterDescriptor.bytesRemaining == 0);

  uint8_t status = twi.master.handleStatus(
      masterDescriptor.pData[masterDescriptor.bytesTransferred], finish );

  if ( status >= Twi::STATUS_STOP )
  {
    if ( status > Twi::STATUS_STOP )
    {
      masterDescriptor.bytesTransferred--;
      masterDescriptor.bytesRemaining++;
    }
    notifyEndOfMasterTransfer();
  }
}

void TwiStream::handleSlaveStatus()
{
  timeout = Timestamp();
  uint8_t data;
  uint8_t status = twi.slave.handleStatus( data, false );
 //     data, slaveDescriptor.bytesRemaining == 1 );

  if ( status < Twi::STATUS_STOP )
  {
    state |= SLAVE_BUSY;
    if ( status == Twi::STATUS_START )
    {
      if ( slaveDescriptor.bytesTransferred )
      {
        // this happens if STOP was not seen
        WARN_1( "unexpected START received!" );
        notifyEndOfSlaveTransfer();
        state |= SLAVE_BUSY;
      }
    }
    if ( !slaveDescriptor.pData || !slaveDescriptor.bytesRemaining )
    {
      WARN_1( FSTR("slaveDescriptor not set!") );
      slaveDescriptor.bytesRemaining = 0;
      return;
    }
    DEBUG_L3( '/', data, '\\' );
    slaveDescriptor.pData[slaveDescriptor.bytesTransferred++] = data;
    slaveDescriptor.bytesRemaining--;
  }
  else
  {
    notifyEndOfSlaveTransfer();
    if ( state == IDLE )
    {
      // give other jobs a chance
      twi.slave.disableInterrupt();
    }
  }
}

bool TwiStream::isBusIdle()
{
  if ( twi.isBusIdle() && sclPin.isSet() && sdaPin.isSet() )
  {
    return true;
  }
  /*
   // handle possible electrical errors
   if( (twi.isBusUnknown() ||  twi.isBusBusy()) && (retries < MAX_RETRIES) )
   {
   Timestamp tm;
   uint8_t isSet = sclPin.isSet();
   while( !tm.elapsed( TIMEOUT_UNKONWN_STATE ) )
   {
   // check bus for some ms, it must be idle if the pins does not change
   if( isSet != sclPin.isSet() )
   {
   break;
   }
   }
   bool idle = !tm.elapsed( TIMEOUT/8 );
   if( idle )
   {
   init();
   notifyError( RESET_BUS );
   }
   WARN_1( ( idle ? FSTR("Assume IDLE in UNKNOWN or BUSY state") : FSTR("not IDLE in UNKNOWN or BUSY state") ) );
   return idle;
   }
   */
  return false;
}

bool TwiStream::isMyTimeSlot()
{
  return isBusIdle() && lastIdleTimestamp.elapsed( HACF::getDeviceId() & 0xF );
}

bool TwiStream::notifyEvent( const Event& event )
{
  if ( event.isEvWakeup() )
  {
    run();
    reportGatewayLoad();
  }
  else if ( event.isEvMessage() )
  {
    DEBUG_H1( FSTR(".evMessage") );
    return handleRequest( event.isEvMessage()->getMessage() );
  }
  else if ( event.isEvGatewayMessage() )
  {
    DEBUG_H1( FSTR(".evGatewayMessage") );
    HACF* message = event.isEvGatewayMessage()->getMessage();
    if ( DebugOptions::gatewaysReadOnly()
        || !isPartOfThisNetwork(
            message->controlFrame.getReceiverId()->getDeviceId() ) )
    {
      WARN_1( FSTR("read only!") );
      delete message;
    }
    else
    {
      if ( !itsMessageQueue.push( message ) )
      {
        delete message;
        itsMessageQueue.popBack( message );
        delete message;
        notifyError( Gateway::MSG_QUEUE_OVERRUN );
      }
    }
  }
  return true;
}

void TwiStream::run()
{
  if ( inStartUp() )
  {
    DEBUG_H1( FSTR(".startup") );
    init();
//    checkBusTimingCmd.reset();
    setMainState( RUNNING );
    setSleepTime( 100 );
  }
  else if ( inRunning() )
  {
    if( checkPerformanceTimestamp.isValid() && checkPerformanceTimestamp.elapsed( CHECK_PERFORMANCE_TIMEOUT ) )
    {
      sendPerformanceResults();
    }
//    if ( checkBusTimingCmd.isValid() && checkBusTimingCmd.elapsed( 20 ) )
//    {
//      if ( itsMessageQueue.isEmpty() )
//      {
//        checkBusTimingCmd.reset();
//        checkBusTiming( true );
//        return;
//      }
//      else
//      {
//        checkBusTimingCmd = Timestamp();
//      }
//    }

    uint8_t buffer[HACF_MAX_LENGTH];

    // setup slaveDescriptor
    slaveDescriptor.pData = buffer;
    slaveDescriptor.bytesTransferred = 0;
    slaveDescriptor.bytesRemaining = sizeof(buffer);
    twi.slave.enableInterrupt();

    if ( !itsMessageQueue.isEmpty() && isMyTimeSlot() )
    {
      // setup master
      masterDescriptor.pData = &buffer[sizeof(buffer)
          - itsMessageQueue.front()->getLength()];
      masterDescriptor.bytesRemaining = itsMessageQueue.front()->getLength();
      masterDescriptor.bytesTransferred = 0;

      // prepare buffer
      memcpy( masterDescriptor.pData, itsMessageQueue.front(),
              itsMessageQueue.front()->getLength() );
      Header* header = (Header*) masterDescriptor.pData;
      header->address = 0;
      header->checksum = 0;
      header->lastDeviceId = HACF::deviceId;
      header->checksum = Checksum::get( masterDescriptor.pData,
                                        masterDescriptor.bytesRemaining );

      DEBUG_M1( FSTR("start next trx " ) );
      twi.master.startTransfer( *masterDescriptor.pData );
      state |= MASTER_BUSY;
      twi.master.enableInterrupt();

    }TRACE_PORT_SET( Pin0 );
    timeout = Timestamp();
    while ( 1 )
    {
      CriticalSection doNotInterrupt;
      if ( timeout.elapsed( TIMEOUT ) )
      {
        TRACE_PORT_SET( Pin1 );
        ERROR_1( FSTR("TIMEOUT") );
        twi.master.disable();
        twi.slave.disable();
        state = IDLE;
        setMainState( FAILTURE );

        IoPort& port = sdaPin.getIoPort();
        port.setPinsAsOutput( Pin0 | Pin1 );
        port.clearPins( Pin0 | Pin1 );
        setSleepTime( TIMEOUT );
      }
      if ( state == IDLE )
      {
        twi.master.disableInterrupt();
        twi.slave.disableInterrupt();
        break;
      }
    }TRACE_PORT_CLEAR( Pin0 );
  }
  else
  {
    TRACE_PORT_CLEAR( Pin1 );
    ERROR_1( FSTR("FAILTURE") );
    IoPort& port = sdaPin.getIoPort();
    if ( port.getOutputPins() & (Pin0 | Pin1) )
    {
      port.configure( Pin0 | Pin1, PORT_OPC_PULLUP_gc );
      port.setPinsAsInput( sdaPin.getPin() );
      SystemTime::waitMs( 2 );
      port.setPinsAsInput( sclPin.getPin() );
    }

    if ( !sclPin.isSet() || !sdaPin.isSet() )
    {
      return;
    }

    setSleepTime( TIMEOUT );
    if( --busyBusRetries == 0 )
    {
      notifyError( BUSY_BUS );
      busyBusRetries = MAX_RETRIES;
      setMainState( STARTUP );
    }
    else
    {
      init();
      setMainState( RUNNING );
    }
  }
}

void TwiStream::notifyEndOfMasterTransfer()
{
  DEBUG_H1( FSTR(".EoT Master") );

  if ( masterDescriptor.bytesTransferred && masterDescriptor.bytesRemaining
      && --retries )
  {
    ERROR_1( FSTR("unable to send : ") );
    DEBUG_M2( FSTR("td->trans  "), masterDescriptor.bytesTransferred );
    DEBUG_M2( FSTR("td->remain "), masterDescriptor.bytesRemaining );
  }
  else
  {
    HACF* msg;
    if ( itsMessageQueue.pop( msg ) )
    {
      notifyMessageSent( msg );
      delete msg;
    }

    if ( masterDescriptor.bytesRemaining && !retries )
    {
      notifyError( Gateway::WRITE_FAILED );
    }
    retries = MAX_RETRIES;
  }
  masterDescriptor.reset();
  state &= ~MASTER_BUSY;
  lastIdleTimestamp = Timestamp();
  TRACE_PORT_CLEAR( Pin2 );
}

void TwiStream::notifyEndOfSlaveTransfer()
{
  DEBUG_H1( FSTR(".EoT Slave") );
  uint8_t checksum = -1;
  bool notRelevant = false;
  bool readFailed = false;
  uint16_t transferred = slaveDescriptor.bytesTransferred;
  const uint8_t minLength = sizeof(HACF)
      - HACF::ControlFrame::DEFAULT_DATA_LENGTH;

  if ( transferred > minLength )
  {
    Header* hdr = (Header*) slaveDescriptor.pData;
    HACF* hacf = (HACF*) slaveDescriptor.pData;
    checksum = Checksum::get( slaveDescriptor.pData, transferred );
    notRelevant = ((hdr->lastDeviceId == HACF::deviceId)
        || ((numOfGateways < 2)
            && (!hacf->controlFrame.isRelevantForComponent()
                || hacf->controlFrame.isForBootloader())));
    if ( (checksum == 0) && (transferred == hacf->getLength()) && !notRelevant )
    {
      notifyMessageReceived( hacf->getControlFrame() );
      DEBUG_M1( FSTR("SUCCESS") );
    }
    else
    {
      readFailed = !notRelevant;
    }
  }
  else if ( transferred )
  {
    readFailed = true;
  }

  if ( readFailed )
  {
    notifyError( Gateway::READ_FAILED );
    DEBUG_H2( FSTR("transferError CS: "), checksum );
    DEBUG_M2( FSTR("td->trans  "), transferred );
    DEBUG_M3( FSTR("td->remain "), slaveDescriptor.bytesRemaining, endl );
    for ( uint16_t i = 0; i < transferred; i++ )
    {
      DEBUG_L2( '-', slaveDescriptor.pData[i] );
    }
  }

  if ( notRelevant )
  {
    DEBUG_M1( FSTR("not relevant") );
  }
  slaveDescriptor.bytesRemaining += transferred;
  slaveDescriptor.bytesTransferred = 0;
  lastIdleTimestamp = Timestamp();
  state &= ~SLAVE_BUSY;
}

bool TwiStream::handleRequest( HACF* message )
{
  bool consumed = true;
  HACF::ControlFrame& cf = message->controlFrame;
//  if ( cf.isCommand( Command::CHECK_BUS_TIMING ) )
//  {
//    DEBUG_H1( FSTR(".checkBusTiming()") );
//    if ( cf.isBroadcast() )
//    {
//      checkBusTiming();
//    }
//    else
//    {
//      HACF* newMsg = message->copy();
//      if ( newMsg )
//      {
//        newMsg->controlFrame.receiverId.setDeviceId( HACF::BROADCAST_ID );
//        newMsg->controlFrame.senderId.setDeviceId( HACF::deviceId );
//        newMsg->controlFrame.senderId.setObjectId( getId() );
//        checkBusTimingCmd = Timestamp();
//        itsMessageQueue.push( newMsg );
//      }
//    }
//
//  }
//  else if ( cf.isCommand( Command::GET_BUS_TIMING ) )
//  {
//    DEBUG_H1( FSTR(".getBusTiming()") );
//    Response response( getId(), *message );
//    response.setBusTiming( busTiming );
//    response.queue( getObject( message->header.getSenderId() ) );
//  }
//  else if ( cf.isCommand( Command::RESET_BUS_TIMING ) )
//  {
//    resetBusTiming();
//  }
//  else
  if ( cf.isCommand( Command::GET_CONNECTED_DEVICES ) )
  {
    DEBUG_H1( FSTR(".getConnectedDevices()") );
    Response response( getId(), *message );
    response.setConnectedDevices( deviceArray );
    response.queue( getObject( message->header.getSenderId() ) );
  }
  else if ( cf.isCommand( Command::CHECK_BUS_PERFORMANCE ) )
  {
    DEBUG_H1( FSTR(".checkBusPerformance()") );
    if ( deviceArray )
    {
      sentPingIds->clear();
      checkPerformanceTimestamp = Timestamp();
      IResponse ping( getId() );
      ping.getControlFrame()->getReceiverId()->setObjectId( HACF::SYSTEM_ID );
      ping.getControlFrame()->setDataLength( 1 );
      ping.setResponse( HomeAutomationInterface::Command::PING );
      for ( uint8_t i = 0; i < deviceArray->size(); i++ )
      {
        HACF* newMsg = ping.copy();
        if ( newMsg )
        {
          uint16_t receiverId = deviceArray->getElement( i );
          sentPingIds->add( receiverId );
          newMsg->controlFrame.receiverId.setDeviceId( receiverId );
          itsMessageQueue.push( newMsg );
        }
      }
    }
  }
  else if ( cf.isResult( HomeAutomationInterface::Response::PONG ) )
  {
    sentPingIds->remove( cf.senderId.getDeviceId() );
    if( sentPingIds->size() == 0 )
    {
      sendPerformanceResults();
    }
  }
  else
  {
    return false;
  }

  return consumed;
}

static const uint16_t TRIGGER_BUS_TIME_CNT = COUNT_DELAY_BIT_US( 50 );
static const uint16_t MEASURE_BUS_TIME_CNT = 20 * TRIGGER_BUS_TIME_CNT;

void TwiStream::measureBusLine( DigitalOutput& busLine, uint16_t* times )
{
  uint8_t* port = ((uint8_t*) &busLine.getIoPort()) + 8;
  uint8_t pin = busLine.getPin();

  uint8_t i = MAX_BUS_TIMES;
  while ( i-- )
  {
    delayBit( MEASURE_BUS_TIME_CNT, port, pin, 1 );
    delayBit( MEASURE_BUS_TIME_CNT, port, pin, 0 );
    *times++ = (MEASURE_BUS_TIME_CNT
        - delayBit( MEASURE_BUS_TIME_CNT, port, pin, 1 ));
  }
}

void TwiStream::triggerBusLine( DigitalOutput& busLine, uint16_t* times )
{
  uint8_t* port = ((uint8_t*) &busLine.getIoPort()) + 8;
  uint8_t pin = busLine.getPin();
  busLine.configInput();
  _delay_us( 50 );

  uint8_t i = MAX_BUS_TIMES;
  while ( i-- )
  {
    busLine.configOutput();
    delayBit( TRIGGER_BUS_TIME_CNT, port, pin, 1 );
    busLine.configInput();
    uint16_t remaining = delayBit( TRIGGER_BUS_TIME_CNT, port, pin, 1 );
    *times++ = 2 * TRIGGER_BUS_TIME_CNT - remaining;
    delayBit( remaining, port, pin, 1 );
  }
}

void TwiStream::checkBusTiming( bool active )
{
  twi.master.disable();
  twi.slave.disable();
  resetBusTiming();

  if ( active )
  {
    // sync all bus members
    sclPin.configOutput();
    _delay_us( 50 );

    // stimulate the bus lines
    triggerBusLine( sclPin, busTiming.sclTimes );
    triggerBusLine( sdaPin, busTiming.sdaTimes );
  }
  else
  {
    // wait for the first falling edge
    Timestamp timeout;
    while ( sclPin.isSet() )
    {
      if ( timeout.elapsed( TIMEOUT ) )
      {
        break;
      }
    }
    if ( !sclPin.isSet() )
    {
      measureBusLine( sclPin, busTiming.sclTimes );
      measureBusLine( sdaPin, busTiming.sdaTimes );
    }
  }
  init();
}

void TwiStream::resetBusTiming()
{
  uint8_t i = MAX_BUS_TIMES;
  while ( i-- )
  {
    busTiming.sclTimes[i] = 0;
    busTiming.sdaTimes[i] = 0;
  }
}

void TwiStream::sendPerformanceResults()
{
  Response busPerformance( getId() );
  busPerformance.setBusPerformance( checkPerformanceTimestamp.since(), sentPingIds );
  busPerformance.queue();
  checkPerformanceTimestamp.reset();
}

Stream::TransferDescriptor* TwiStream::getMasterDescriptor() const
{
  return (Stream::TransferDescriptor*) &masterDescriptor;
}

DigitalOutput* TwiStream::getSclPin() const
{
  return (DigitalOutput*) &sclPin;
}

DigitalOutput* TwiStream::getSdaPin() const
{
  return (DigitalOutput*) &sdaPin;
}

Stream::TransferDescriptor* TwiStream::getSlaveDescriptor() const
{
  return (Stream::TransferDescriptor*) &slaveDescriptor;
}

