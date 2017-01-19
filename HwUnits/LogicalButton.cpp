/*
 * LogicalButton.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "LogicalButton.h"
#include "Button.h"
#include "Led.h"

const uint8_t LogicalButton::debugLevel( DEBUG_LEVEL_OFF );

LogicalButton::Response::Parameter& LogicalButton::Response::setConfiguration()
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().configuration) );
  setResponse( CONFIGURATION );
  return getParameter();
}

void LogicalButton::Response::setStatus( uint8_t status )
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().status) );
  setResponse( STATUS );
  getParameter().status = status;
}

LogicalButton::LogicalButton( uint8_t _id )
{
  {
    for ( int pos = 0; pos < HwConfiguration::LogicalButton::MAX_OBJECTS;
        ++pos )
    {
      led[pos] = NULL;
    }
  }
  configuration = NULL;
  {
    for ( int pos = 0; pos < HwConfiguration::LogicalButton::MAX_OBJECTS;
        ++pos )
    {
      button[pos] = NULL;
    }
  }
  Object::setId( (ClassId::LOGICAL_BUTTON << 8) | _id );
  configuration = HwConfiguration::getLogicalButtonConfiguration( id );
}

void LogicalButton::clearObjectList()
{
  for ( uint8_t i = 0; i < HwConfiguration::LogicalButton::MAX_OBJECTS; i++ )
  {
    led[i] = 0;
    if ( button[i] )
    {
      button[i]->setFeedbackLed( 0 );
      button[i] = 0;
    }
  }
}

uint8_t LogicalButton::getLedStatus()
{
  for ( uint8_t i = 0; i < HwConfiguration::LogicalButton::MAX_OBJECTS; i++ )
  {
    if ( led[i] )
    {
      if ( led[i]->getState().sub != Led::OFF )
      {
        return Led::ON;
      }
    }
  }
  return Led::OFF;

}

bool LogicalButton::notifyEvent( const Event& event )
{
  if ( event.isEvWakeup() )
  {
    updateObjectList();
    setSleepTime( NO_WAKE_UP );
  }
  else if ( event.isEvMessage() )
  {
    return handleRequest( event );
  }
  return false;
}

void LogicalButton::routeMessageToButton( const Event& event )
{
  bool consumed = true;
  for ( uint8_t i = 0; i < HwConfiguration::LogicalButton::MAX_OBJECTS; i++ )
  {
    if ( button[i] )
    {
      consumed &= button[i]->notifyEvent( event );
    }
  }

  if ( !consumed )
  {
    Response e( getId() );
    e.setErrorCode( ErrorCode::NO_BUTTON );
    e.queue();
  }

}

void LogicalButton::routeMessageToLed( const Event& event )
{
  bool consumed = true;
  for ( uint8_t i = 0; i < HwConfiguration::LogicalButton::MAX_OBJECTS; i++ )
  {
    if ( led[i] )
    {
      consumed &= led[i]->notifyEvent( event );
    }
  }

  if ( !consumed )
  {
    Response e( getId() );
    e.setErrorCode( ErrorCode::NO_LED );
    e.queue();
  }

}

void LogicalButton::updateObjectList()
{
  for ( uint8_t i = 0; i < HwConfiguration::LogicalButton::MAX_OBJECTS; i++ )
  {
    led[i] = (Led*) getObject(
        (ClassId::LED << 8) | configuration->getLedId( i ) );
    button[i] = (Button*) getObject(
        (ClassId::BUTTON << 8) | configuration->getButtonId( i ) );

    if ( button[i] && led[i] )
    {
      button[i]->setFeedbackLed( led[i] );
    }
  }
}

bool LogicalButton::handleRequest( const Event& event )
{
  HACF* message = event.isEvMessage()->getMessage();
  if ( !message->controlFrame.isCommand() )
  {
    return false;
  }
  HACF::ControlFrame& cf = message->controlFrame;
  Command* data = reinterpret_cast<Command*>( cf.getData() );

  if ( cf.isCommand( Command::ON ) )
  {
    DEBUG_H1( FSTR(".on()") );
    routeMessageToLed( event );
  }
  else if ( cf.isCommand( Command::OFF ) )
  {
    DEBUG_H1( FSTR(".off()") );
    routeMessageToLed( event );
  }
  else if ( cf.isCommand( Command::BLINK ) )
  {
    DEBUG_H1( FSTR(".blink()") );
    routeMessageToLed( event );
  }
  else if ( cf.isCommand( Command::SET_MIN_BRIGHTNESS ) )
  {
    DEBUG_H1( FSTR(".setMinBrightness()") );
    routeMessageToLed( event );
  }
  else if ( cf.isCommand( Command::SET_CONFIGURATION ) )
  {
    DEBUG_H1( FSTR(".setConfiguration()") );
    configuration->set( data->parameter.setConfiguration );
    updateObjectList();
  }
  else if ( cf.isCommand( Command::SET_BUTTON_CONFIGURATION ) )
  {
    DEBUG_H1( FSTR(".setButtonConfiguration()") );
    data->setCommand( Button::Command::SET_CONFIGURATION );
    routeMessageToButton( event );
    data->setCommand( Command::SET_BUTTON_CONFIGURATION );
  }
  else if ( cf.isCommand( Command::SET_LED_CONFIGURATION ) )
  {
    DEBUG_H1( FSTR(".setLedConfiguration()") );
    data->setCommand( Led::Command::SET_CONFIGURATION );
    routeMessageToLed( event );
    data->setCommand( Command::SET_LED_CONFIGURATION );
  }
  else
  {
    Response response( getId(), *message );

    if ( cf.isCommand( Command::GET_STATUS ) )
    {
      DEBUG_H1( FSTR(".getStatus()") );
      response.setStatus( getLedStatus() );
    }
    else if ( cf.isCommand( Command::GET_CONFIGURATION ) )
    {
      DEBUG_H1( FSTR(".getConfiguration()") );
      configuration->get( response.setConfiguration().configuration );
    }
    else
    {
      return false;
    }
    response.queue( getObject( message->header.getSenderId() ) );
  }
  return true;
}

void LogicalButton::addButton( Button* p_Button )
{
  for ( int pos = 0; pos < HwConfiguration::LogicalButton::MAX_OBJECTS; ++pos )
  {
    if ( !button[pos] )
    {
      button[pos] = p_Button;
      break;
    }
  }
}

HwConfiguration::LogicalButton* LogicalButton::getConfiguration() const
{
  return configuration;
}

void LogicalButton::setConfiguration(
    HwConfiguration::LogicalButton* p_LogicalButton )
{
  configuration = p_LogicalButton;
}

void LogicalButton::addLed( Led* p_Led )
{
  for ( int pos = 0; pos < HwConfiguration::LogicalButton::MAX_OBJECTS; ++pos )
  {
    if ( !led[pos] )
    {
      led[pos] = p_Led;
      break;
    }
  }
}
