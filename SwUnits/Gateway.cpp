/*
 * Gateway.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Gateway.h"
#include "IResponse.h"
#include "DebugOptions.h"

#include <Security/Checksum.h>
#include <Scheduler.h>

static const uint8_t debugLevel( DEBUG_LEVEL_LOW );

EventDrivenUnit* Gateway::listener( NULL );

uint8_t Gateway::numOfGateways( 0 );

Gateway* Gateway::itsGateway[MAX_GATEWAYS];

void Gateway::addGateway()
{

   for ( uint8_t pos = 0; pos < MAX_GATEWAYS; ++pos )
   {
      if ( !itsGateway[pos] )
      {
         DEBUG_M4( "Gateway: 0x", (uint16_t )this, " added :", numOfGateways );
         itsGateway[pos] = this;
         numOfGateways++;
         break;
      }
   }
}

void Gateway::terminate()
{
   for ( uint8_t pos = 0; pos < MAX_GATEWAYS; ++pos )
   {
      if ( itsGateway[pos] == this )
      {
         numOfGateways--;
         itsGateway[pos] = NULL;
         Reactive::terminate();
      }
   }
}

bool Gateway::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      run();
   }
/*
   else if ( event.isEvMessage() )
   {
    DEBUG_H1( FSTR(".evMessage") );
    handleRequest( event.isEvMessage()->getMessage() );
   }
 */
   else if ( event.isEvGatewayMessage() )
   {
      DEBUG_H1( FSTR( ".evGatewayMessage" ) );
      HACF* message = event.isEvGatewayMessage()->getMessage();
      if ( DebugOptions::gatewaysReadOnly() )
      {
         WARN_1( FSTR( "read only!" ) );
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
   else if ( event.isEvData() )
   {
      Stream::TransferDescriptor* td = event.isEvData()->getTransferDescriptor();

      if ( td->bytesTransferred != 0 )
      {
         // we received new data
         DEBUG_H1( FSTR( "->data received" ) );

         HACF::ControlFrame* msg = (HACF::ControlFrame*) ( td->pData );

         if ( getInstanceId() == UDP )
         {
            if ( *( (uint16_t*) td->pData ) == MAGIC_NUMBER )
            {
               msg = (HACF::ControlFrame*) ( td->pData + sizeof( MAGIC_NUMBER ) );
               if ( msg->getLength()
                    >= ( td->bytesTransferred - sizeof( MAGIC_NUMBER ) ) )
               {
                  notifyMessageReceived( msg );
               }
            }
         }
         else
         {
            notifyEndOfReadTransfer( td );
         }
      }

   }
   else
   {
      return false;
   }
   return true;
}

void Gateway::run()
{
   if ( inStartUp() )
   {
      IoStream::CommandINIT data;
      data.deviceId = HACF::getDeviceId() & 0x7F;
      data.buffersize = HACF::MAX_DATA_SIZE;
      data.owner = this;
      if ( ioStream
         && ( ioStream->genericCommand( IoStream::INIT, &data ) == Stream::SUCCESS ) )
      {
         // configuration = HwConfiguration::getUdpStreamConfiguration( id );
         SET_STATE_L1( RUNNING );
      }
      else
      {
         Gateway::terminate();
         ERROR_1( FSTR( "init ioStream not possible!" ) );
      }
   }

   if ( inRunning() )
   {

      uint8_t buffer[5*HACF::MAX_DATA_SIZE];

      // if there is some data, it will be notified by evData
      Stream::Status status = ioStream->read( buffer, sizeof( buffer ), this );

      if ( !itsMessageQueue.isEmpty() )
      {
         if ( itsMessageQueue.isFull() || lastIdleTime.elapsed( MIN_IDLE_TIME + ( HACF::deviceId& 0xF ) + ( RETRY_DELAY_TIME * retries ) ) )
         {
            uint16_t len = itsMessageQueue.front()->getLength();

            // prepare buffer
            memcpy( buffer, itsMessageQueue.front(), len );

            // message->getControlFrame()->setPacketCounter( packetCounter );
            // message->getControlFrame()->encrypt();

            if ( getInstanceId() == UDP )
            {
               buffer[2] = LBYTE( MAGIC_NUMBER );
               buffer[3] = HBYTE( MAGIC_NUMBER );
               notifyEndOfWriteTransfer( ioStream->write( &buffer[2], len - 2, this ) );
            }
            else
            {
               Header* header = (Header*) buffer;
               header->address = 0;
               header->checksum = 0;
               header->lastDeviceId = HACF::deviceId;
               header->checksum = Checksum::get( buffer, len );
               notifyEndOfWriteTransfer( ioStream->write( buffer, len, this ) );
            }
         }
      }
      reportGatewayLoad();
   }
}

void Gateway::notifyEndOfWriteTransfer( Stream::Status status )
{
   writeStatus[retries] = status;
   HACF* msg;

   if ( ( status != Stream::SUCCESS ) && ( retries < MAX_RETRIES ) )
   {
      if ( ( status == Stream::NO_RECEIVER ) && !retries )
      {
         if ( itsMessageQueue.pop( msg ) )
         {
            delete msg;
         }
      }
      else
      {
         retries++;
         ERROR_2( FSTR( "unable to send : " ), (uint8_t)status );
      }
   }
   else
   {
      if ( itsMessageQueue.pop( msg ) )
      {
         if ( status != Stream::SUCCESS )
         {
            notifyError( Gateway::WRITE_FAILED, (uint8_t*) writeStatus );
         }
         else
         {
            packetCounter++;
            notifyMessageSent( msg );
         }
         delete msg;
      }
      retries = 0;
   }
   lastIdleTime = Timestamp();
}

void Gateway::notifyEndOfReadTransfer( Stream::TransferDescriptor* td )
{
   DEBUG_H1( FSTR( ".EoT Slave" ) );
   uint8_t checksum = -1;
   bool notRelevant = false;
   bool readFailed = false;
   uint16_t transferred = td->bytesTransferred;
   const uint8_t minLength = sizeof( HACF )
                             - HACF::ControlFrame::DEFAULT_DATA_LENGTH;

   if ( transferred > minLength )
   {
      Header* hdr = (Header*) td->pData;
      HACF* hacf = (HACF*) td->pData;
      checksum = Checksum::get( td->pData, transferred );
      notRelevant = ( ( hdr->lastDeviceId == HACF::deviceId )
                    || ( ( numOfGateways < 2 )
                       && ( !hacf->controlFrame.isRelevantForComponent()
                          || hacf->controlFrame.isForBootloader() ) ) );
      if ( ( checksum == 0 ) && ( transferred == hacf->getLength() ) && !notRelevant )
      {
         notifyMessageReceived( hacf->getControlFrame() );
         DEBUG_M1( FSTR( "SUCCESS" ) );
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
      readFailedData.transferred = td->bytesTransferred;
      readFailedData.remaining = td->bytesRemaining;
      readFailedData.checksum = checksum;
      notifyError( Gateway::READ_FAILED, (uint8_t*) &readFailedData );

      DEBUG_H2( FSTR( "transferError CS: " ), checksum );
      DEBUG_M2( FSTR( "td->trans  " ), transferred );
      DEBUG_M3( FSTR( "td->remain " ), td->bytesRemaining, endl );
      for ( uint16_t i = 0; i < transferred; i++ )
      {
         DEBUG_L2( '-', td->pData[i] );
      }
   }

   if ( notRelevant )
   {
      DEBUG_M1( FSTR( "not relevant" ) );
   }
   lastIdleTime = Timestamp();
}

uint8_t Gateway::getDebugLevel()
{
   return debugLevel;
}

uint8_t Gateway::getNumOfGateways()
{
   return numOfGateways;
}

void Gateway::notifyError( uint8_t errorCode, uint8_t* errorData )
{

   IResponse event( getId() );

   /*
      // force notification for this gateway, too
      event->header.senderId = 0;

      if( numOfGateways > 1 )
      {
      event->header.senderId = getId();
      }
    */
   event.setErrorCode( errorCode, errorData );
   event.queue();

}

void Gateway::notifyMessageReceived( HACF::ControlFrame* controlFrame )
{
   DEBUG_H1( FSTR( ".new Msg " ) );
   HACF* msg = (HACF*) ( ( (uint8_t*) controlFrame ) - sizeof( HACF::Header ) );

   HACF* newMsg = msg->copy();
   if ( newMsg )
   {
      newMsg->header.setSenderId( getId() );
      if ( listener )
      {
         evGatewayMessage( listener, newMsg ).send();
      }

      if ( !evGatewayMessage( Scheduler::getJob( HACF::SYSTEM_ID ), newMsg ).queue() )
      {
         ERROR_1( FSTR( "EventQueue is full" ) );
         delete newMsg;
      }
   }

   gatewayLoad.messagesPerMinute++;
   gatewayLoad.bytesPerMinute += msg->getLength();
}

void Gateway::notifyMessageSent( HACF* hacf )
{
   DEBUG_H1( FSTR( "->msg sent" ) );
   gatewayLoad.messagesPerMinute++;
   gatewayLoad.bytesPerMinute += hacf->getLength();
   if ( listener )
   {
      evGatewayMessage( listener, hacf ).send();
   }
}

void Gateway::setNumOfGateways( uint8_t p_numOfGateways )
{

   numOfGateways = p_numOfGateways;

}

void Gateway::Response::setGatewayLoad( GatewayLoad& gatewayLoad )
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().gatewayLoad ) );
   setResponse( EVENT_GATEWAY_LOAD );
   getParameter().gatewayLoad = gatewayLoad;
}

void Gateway::reportGatewayLoad()
{
   if ( lastReportTimestamp.since() > SystemTime::MIN )
   {
      if ( DebugOptions::notifyMessageLoad() )
      {
         Response event( getId() );
         event.setGatewayLoad( gatewayLoad );
         event.queue();
      }
      lastReportTimestamp = Timestamp();
      gatewayLoad.bytesPerMinute = 0;
      gatewayLoad.messagesPerMinute = 0;
   }
}
