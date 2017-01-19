/*
 * IrReceiver.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "IrReceiver.h"
#include <PortPin.h>

extern void configureInfraRedHw( PortPin portPin, IrDecoder* decoder );

const uint8_t IrReceiver::debugLevel( DEBUG_LEVEL_OFF );

extern void configureInfraRedHw( PortPin portPin, IrDecoder* decoder );

extern void configureInfraRedHw( PortPin portPin, IrDecoder* decoder );

IrReceiver::IrReceiver( uint8_t _id, PortPin portPin ) :
    receiving( 0 ), repetitionFrameNumber( 0 ),
    inputPin( portPin.getPortNumber(), portPin.getPinNumber() )
{
  setId( (ClassId::IR_RECEIVER << 8) | _id );
}

bool IrReceiver::notifyEvent( const Event& event )
{
  if ( event.isEvWakeup() )
  {
    run();
  }
  else if ( event.isEvMessage() )
  {
    return handleRequest( event.isEvMessage()->getMessage() );
  }
  return false;

}

void IrReceiver::run()
{
  if ( inStartUp() )
  {
    configureInfraRedHw( inputPin, &irDecoder );
    setMainState( RUNNING );
  }
  else if ( inRunning() )
  {
    checkForNewEvent();
    setSleepTime( 10 );
  }
  else
  {
    setSleepTime( NO_WAKE_UP );
  }
}

void IrReceiver::checkForNewEvent()
{
  IrDecoder::Data data;
  bool newData;
  CriticalSection doNotInterrupt;
  {
    newData = irDecoder.getData( data );
    if ( newData )
    {
      receiving = TIMEOUT;
    }
    else if ( receiving )
    {
      if ( !--receiving )
      {
        newData = irDecoder.notifyTimeout();
        irDecoder.getData( data );
      }
    }
  }

  if ( newData )
  {
    if ( (data.repetitionFrameNumber == 0)
        || (data.repetitionFrameNumber == 3) )
    {
      Response irEvent( getId() );
      if ( data.repetitionFrameNumber == 0 )
      {
        if ( repetitionFrameNumber >= 3 )
        {
          irEvent.setResponse( Response::EVENT_HOLD_END );
        }
        else
        {
          irEvent.setResponse( Response::EVENT_CLICKED );
        }
      }
      else
      {
        irEvent.setResponse( Response::EVENT_HOLD_START );
      }
      irEvent.setData( data );

      DEBUG_H1( FSTR(".evData()") );
      DEBUG_M2( FSTR("a: "), data.address );
      DEBUG_M2( FSTR("c: "), data.command );
      irEvent.queue();
    }
    repetitionFrameNumber = data.repetitionFrameNumber;
    DEBUG_M2( FSTR("r: "), repetitionFrameNumber );
  }
}

bool IrReceiver::handleRequest( HACF* message )
{
  if ( !message->controlFrame.isCommand() )
  {
    return false;
  }
  HACF::ControlFrame& cf = message->controlFrame;
  if ( cf.isCommand( Command::ON ) )
  {
    DEBUG_H1( FSTR(".on()") );
  }
  else if ( cf.isCommand( Command::OFF ) )
  {
    DEBUG_H1( FSTR(".off()") );
  }
  else
  {
    return false;
  }

  return true;
}

DigitalInput* IrReceiver::getInputPin() const
{
  return (DigitalInput*) &inputPin;
}

IrDecoder* IrReceiver::getIrDecoder() const
{
  return (IrDecoder*) &irDecoder;
}
