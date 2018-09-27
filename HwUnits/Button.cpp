/*
 * Button.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Button.h"
#include "Led.h"

const uint8_t Button::debugLevel( DEBUG_LEVEL_OFF );

Button::Response::Parameter& Button::Response::setConfiguration()
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().configuration ) );
   setResponse( CONFIGURATION );
   return getParameter();
}

Button::Button( uint8_t _id ) :
   disabledDuration( 0 )
{
   feedbackLed = NULL;
   configuration = NULL;
   setMainState( RUNNING );
   setSubState( RELEASED );
   Object::setId( ( ClassId::BUTTON << 8 ) | _id );
   setConfiguration( ConfigurationManager::getConfiguration<EepromConfiguration>( id ) );
   enabledEvents = configuration->getEvents();

}

bool Button::handleRequest( HACF* message )
{
   HACF::ControlFrame& cf = message->controlFrame;
   Command* data = reinterpret_cast<Command*>( cf.getData() );

   if ( cf.isCommand( Command::GET_CONFIGURATION ) )
   {
      DEBUG_H1( FSTR( ".getConfiguration()" ) );
      Response response( getId(), *message );
      configuration->get( response.setConfiguration().configuration );
      response.queue( getObject( message->header.getSenderId() ) );
   }
   else if ( cf.isCommand( Command::GET_STATUS ) )
   {
      DEBUG_H1( FSTR( ".getStatus()" ) );
      Response response( getId(), *message );
      response.setState( state.sub );
      response.queue( getObject( message->header.getSenderId() ) );
   }
   else if ( cf.isCommand( Command::SET_CONFIGURATION ) )
   {
      DEBUG_H1( FSTR( ".setConfiguration()" ) );
      configuration->set( data->parameter.setConfiguration );
      enabledEvents = configuration->getEvents();
   }
   else if ( cf.isCommand( Command::ENABLE_EVENTS ) )
   {
      Command::EnableEvents& ee = data->parameter.enableEvents;
      DEBUG_H5( FSTR( ".enableEvents(" ), ee.enable, ',', ee.disabledDuration, ')' );
      bool enable = ( ee.enable == Command::True );
      if ( ee.enable == Command::Invert )
      {
         enable = ( enabledEvents.mask == 0 );
      }
      setDisabledDuration( enable ? 0 : ee.disabledDuration );
      enabledEvents.mask = ( enable ? configuration->getEvents().mask : 0 );
      DEBUG_M2( FSTR( "enabledEvents    :" ), enabledEvents.mask );
      DEBUG_M2( FSTR( "disabledDuration :" ), getDisabledDuration() );
   }
   else
   {
      return false;
   }

   return true;
}

bool Button::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      tick();
   }
   else if ( event.isEvMessage() )
   {
      return handleRequest( event.isEvMessage()->getMessage() );
   }

   return false;
}

void Button::tick()
{
   if ( disabledDuration != 0 )
   {
      setSleepTime( SystemTime::S );
      if ( --disabledDuration == 0 )
      {
         setSleepTime( NO_WAKE_UP );
         enabledEvents.mask = configuration->getEvents().mask;
      }
      return;
   }

   Configuration::Event event;
   event.mask = 0;

   if ( inSubState( PRESSED ) )
   {
      setSubState( HOLD );
      event.bit.notifyOnHoldStart = 1;
   }
   else if ( inSubState( CLICKED ) )
   {
      setSubState( RELEASED );
      event.bit.notifyOnClicked = 1;
   }
   else if ( inSubState( CLICKED_PRESSED ) )
   {
      setSubState( HOLD );
      event.bit.notifyOnClicked = 1;
      event.bit.notifyOnHoldStart = 1;
   }
   STATE_L2( state );
   notifyNewState( event );
   setSleepTime( NO_WAKE_UP );
}

void Button::updateState( uint8_t pressed )
{
   if ( enabledEvents.mask )
   {
      Configuration::Event event;
      event.mask = 0;

      if ( pressed )
      {
         DEBUG_H1( FSTR( "buttonPress" ) );
         event.bit.notifyOnCovered = 1;
         {
            if ( enabledEvents.bit.feedbackEnabled && feedbackLed )
            {
               feedbackLed->enableFeedback();
            }
         }
         if ( inSubState( RELEASED ) )
         {
            setSubState( PRESSED );
            if ( enabledEvents.bit.notifyOnHoldStart )
            {
               setSleepTime( configuration->holdTimeout * 10 );
            }
         }
         else if ( inSubState( CLICKED ) )
         {
            setSubState( CLICKED_PRESSED );
            setSleepTime( configuration->holdTimeout * 10 );
         }

      }
      else
      {
         DEBUG_H1( FSTR( "buttonRelease" ) );
         event.bit.notifyOnFree = 1;
         {
            if ( enabledEvents.bit.feedbackEnabled && feedbackLed )
            {
               feedbackLed->enableFeedback( false );
            }
         }
         if ( inSubState( PRESSED ) )
         {
            if ( enabledEvents.bit.notifyOnDoubleClicked )
            {
               setSubState( CLICKED );
               setSleepTime( configuration->doubleClickTimeout * 10 );
            }
            else
            {
               setSubState( RELEASED );
               event.bit.notifyOnClicked = 1;
               setSleepTime( NO_WAKE_UP );
            }
         }
         else if ( inSubState( HOLD ) )
         {
            setSubState( RELEASED );
            event.bit.notifyOnHoldEnd = 1;
         }
         else if ( inSubState( CLICKED_PRESSED ) )
         {
            setSubState( RELEASED );
            event.bit.notifyOnDoubleClicked = 1;
            setSleepTime( NO_WAKE_UP );
         }
      }
      STATE_L2( state );
      notifyNewState( event );
   }
}

void Button::notifyNewState( Configuration::Event event ) const
{
   event.mask &= enabledEvents.mask;

   Response eventMessage( getId() );
   while ( event.mask )
   {
      if ( event.bit.notifyOnCovered )
      {
         DEBUG_H1( FSTR( ".evCovered" ) );
         event.bit.notifyOnCovered = 0;
         eventMessage.setEvent( Response::EVENT_COVERED );
      }
      else if ( event.bit.notifyOnFree )
      {
         DEBUG_H1( FSTR( ".evFree" ) );
         event.bit.notifyOnFree = 0;
         eventMessage.setEvent( Response::EVENT_FREE );
      }
      else if ( event.bit.notifyOnClicked )
      {
         DEBUG_H1( FSTR( ".evClicked" ) );
         event.bit.notifyOnClicked = 0;
         eventMessage.setEvent( Response::EVENT_CLICKED );
      }
      else if ( event.bit.notifyOnDoubleClicked )
      {
         DEBUG_H1( FSTR( ".evDoubleClicked" ) );
         event.bit.notifyOnDoubleClicked = 0;
         eventMessage.setEvent( Response::EVENT_DOUBLE_CLICKED );
      }
      else if ( event.bit.notifyOnHoldStart )
      {
         DEBUG_H1( FSTR( ".evHoldStart" ) );
         event.bit.notifyOnHoldStart = 0;
         eventMessage.setEvent( Response::EVENT_HOLD_START );
      }
      else if ( event.bit.notifyOnHoldEnd )
      {
         DEBUG_H1( FSTR( ".evHoldEnd" ) );
         event.bit.notifyOnHoldEnd = 0;
         eventMessage.setEvent( Response::EVENT_HOLD_END );
      }
      else
      {
         break;
      }
      eventMessage.queue();
   }
}


Led* Button::getFeedbackLed() const
{
   return feedbackLed;
}

void Button::setFeedbackLed( Led* p_Led )
{
   feedbackLed = p_Led;
}
