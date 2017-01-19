/*
 * Counter.cpp
 *
 *  Created on: 22.10.2015
 *      Author: Viktor Pankraz
 */

#include <ErrorMessage.h>
#include "MonitoredDigitalInput.h"

const uint8_t MonitoredDigitalInput::debugLevel( DEBUG_LEVEL_LOW );

MonitoredDigitalInput::Response::Parameter& MonitoredDigitalInput::Response::setConfiguration()
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().configuration) );
  setResponse( CONFIGURATION );
  return getParameter();
}

MonitoredDigitalInput::MonitoredDigitalInput( PortPin _hardware ) :
    hardware( _hardware )
{
  Object::setId(
      (ClassId::MONITORED_DIGITAL_INPUT << 8)
          | ((_hardware.getPortNumber() + 1) << 4)
          | (_hardware.getPinNumber() + 1) );
  configuration = HwConfiguration::getMonitoredDigitalInputConfiguration( id );
  if ( configuration )
  {

  }
  else
  {
    terminate();
    ErrorMessage( getId(), ErrorMessage::CONFIGURATION_OUT_OF_MEMORY );
  }
}

bool MonitoredDigitalInput::handleRequest( HACF* message )
{
  HACF::ControlFrame& cf = message->controlFrame;
  Command* data = reinterpret_cast<Command*>( cf.getData() );

  if ( cf.isCommand( Command::GET_CONFIGURATION ) )
  {
    DEBUG_H1( FSTR(".getConfiguration()") );
    Response response( getId(), *message );
    configuration->get( response.setConfiguration().configuration );
    response.queue( getObject( message->header.getSenderId() ) );
  }
  else if ( cf.isCommand( Command::GET_STATUS ) )
  {
    DEBUG_H1( FSTR(".getStatus()") );
    Response response( getId(), *message );
    response.setStatus( (State) state.sub );
    response.queue( getObject( message->header.getSenderId() ) );
  }
  else if ( cf.isCommand( Command::SET_CONFIGURATION ) )
  {
    DEBUG_H1( FSTR(".setConfiguration()") );
    configuration->set( data->parameter.setConfiguration );
  }
  else
  {
    return false;
  }

  return true;
}

bool MonitoredDigitalInput::notifyEvent( const Event& event )
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

MonitoredDigitalInput::State MonitoredDigitalInput::getCurrentState()
{
  uint16_t remaining = hardware.getResponseTime();
  DEBUG_H2( FSTR(".getCurrentState() 0x"), remaining );
  if ( !remaining )
  {
    return MANIPULATED_OPEN;
  }
  else if ( remaining >= configuration->getUpperThreshold() )
  {
    return MANIPULATED_SHORTEN;
  }
  else if ( remaining >= configuration->getLowerThreshold() )
  {
    return LOW;
  }
  return HIGH;
}

void MonitoredDigitalInput::notifyNewState( State _state )
{
  Response eventMessage( getId() );
  eventMessage.setEvent( _state + HACF::EVENTS_START );
  eventMessage.queue();
}

void MonitoredDigitalInput::run()
{
  if ( inStartUp() )
  {
    SET_STATE_L1( RUNNING );
    SET_STATE_L2( NOT_INITIALIZED );
  }

  State currentState = getCurrentState();
  if ( currentState != (State) state.sub )
  {
    notifyNewState( currentState );
    SET_STATE_L2( currentState );
  }
  setSleepTime( 10* SystemTime::MS );
}
