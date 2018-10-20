/*
 * Led.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Led.h"
#include <PortPin.h>
#include <PwmOutput.h>

const uint8_t Led::debugLevel( DEBUG_LEVEL_OFF | DEBUG_STATE_L3 );

void Led::Response::setBrightness( uint8_t brightness )
{
   if ( brightness )
   {
      setResponse( EVENT_ON );
      getParameter().status = brightness;
   }
   else
   {
      controlFrame.setDataLength( sizeof( getResponse() ) );
      setResponse( EVENT_OFF );
   }
}

Led::Response::Parameter& Led::Response::setConfiguration()
{
   controlFrame.setDataLength( sizeof( getResponse() ) + sizeof( getParameter().configuration ) );
   setResponse( CONFIGURATION );
   return getParameter();
}

void Led::Response::setEvent( uint8_t event )
{
   controlFrame.setDataLength( sizeof( getResponse() ) );
   setResponse( event );
}

void Led::Response::setStatus( uint8_t status )
{
   controlFrame.setDataLength( sizeof( getResponse() ) + sizeof( getParameter().status ) );
   setResponse( STATUS );
   getParameter().status = status;
}

Led::Led( PortPin _hardware ) :
   PortPinUnit( _hardware ), brightness( 0 ), dimmOffset( 0 ),
   minBrightness( 0 ), brightnessBeforeBlink( 0 ), stateBeforeBlink( OFF )
{
   Object::setId(
      ( ClassId::LED << 8 ) | ( ( _hardware.getPortNumber() + 1 ) << 4 )
      | ( _hardware.getPinNumber() + 1 ) );
}

void Led::cmdBlink( const Led::Command::Blink& parameter )
{
   // save previous brightness only from static state
   if ( quantity == 0 )
   {
      brightnessBeforeBlink = brightness;
      stateBeforeBlink = state.sub;
   }
   SET_STATE_L2( OFF );
   setBrightness( parameter.brightness );
   cmdToggle( parameter.toggleParams );
   if ( quantity >= 0x80 )
   {
      quantity = 0xFF;
   }
   quantity <<= 1;
}

void Led::cmdOn( const Led::Command::On& parameter )
{
   setDuration( parameter.duration );
   setBrightness( parameter.brightness );
   setQuantity( duration ? 1 : 0 );
   setSleepTime( duration ? configuration->timeBase : NO_WAKE_UP );

   if ( brightness )
   {
      SET_STATE_L2( ON );
   }
   else
   {
      SET_STATE_L2( OFF );
   }
}

void Led::enableFeedback( bool enable )
{
   feedback = enable;
   updateHw();
}

uint8_t Led::getHwBrightness() const
{
   if ( getFeedback() )
   {
      return FEEDBACK_BRIGHTNESS;
   }
   if ( ( brightness < dimmOffset ) || ( state.sub == OFF ) )
   {
      return minBrightness;
   }

   return maximum<uint8_t>( brightness - dimmOffset, minBrightness );
}

bool Led::handleRequest( HACF* message )
{
   if ( !message->controlFrame.isCommand() )
   {
      return false;
   }
   HACF::ControlFrame& cf = message->controlFrame;
   Command* data = reinterpret_cast<Command*>( cf.getData() );

   if ( cf.isCommand( Command::SET_CONFIGURATION ) )
   {
      DEBUG_H1( FSTR( ".setConfiguration()" ) );
      configuration->set( data->parameter.setConfiguration );
      setDimmOffset( configuration->dutyOffset );
      updateConfiguration();
   }
   else if ( cf.isCommand( Command::SET_MIN_BRIGHTNESS ) )
   {
      setMinBrightness( data->parameter.setMinBrightness.brightness );
      DEBUG_H3( FSTR( ".minBrightness(" ), minBrightness, ')' );
   }
   else
   {
      if ( cf.isCommand( Command::GET_CONFIGURATION )
         || cf.isCommand( Command::GET_STATUS ) )
      {
         // handled by SuperClass
         return false;
      }
      else if ( cf.isCommand( Command::OFF ) )
      {
         setBrightness( minBrightness );
         // forward to SuperClass
         return false;
      }
      else
      {
         Response response( getId() );
         if ( cf.isCommand( Command::ON ) )
         {
            DEBUG_H1( FSTR( ".on()" ) );
            cmdOn( data->parameter.on );
            response.setBrightness( brightness );
         }
         else if ( cf.isCommand( Command::BLINK ) )
         {
            DEBUG_H1( FSTR( ".blink()" ) );
            cmdBlink( data->parameter.blink );
            response.setEvent( Response::EVENT_BLINK );
         }
         else
         {
            return false;
         }
         response.queue();
      }
   }
   return true;
}

bool Led::notifyEvent( const Event& event )
{
   bool consumed = false;
   if ( event.isEvWakeup() )
   {
      bool updateConf = inStartUp();
      bool isBlinking = ( quantity != 0 );
      bool hasNewEvent = run();
      if ( hasNewEvent )
      {
         // restore previous state if blinking ends
         if ( isBlinking && !quantity )
         {
            state.sub = stateBeforeBlink;
            brightness = brightnessBeforeBlink;
         }

         // notify about the new static state
         Response event( getId() );
         event.setBrightness( ( state.sub == ON ) ? brightness : 0 );
         event.queue();

      }
      if ( updateConf && configuration )
      {
         setMinBrightness( configuration->minDuty );
         setDimmOffset( configuration->dutyOffset );
      }
   }
   else if ( event.isEvMessage() )
   {
      HACF* message = event.isEvMessage()->getMessage();
      consumed = handleRequest( message );
      if ( !consumed )
      {
         consumed = PortPinUnit::handleRequest( message );
      }
   }

   updateHw();
   return consumed;
}

void Led::setBrightness( uint8_t p_brightness )
{
   if ( p_brightness > MAX_BRIGHTNESS )
   {
      p_brightness = MAX_BRIGHTNESS;
   }
   brightness = p_brightness;
}

void Led::updateHw()
{
   ( (PwmOutput*) &hardware )->set( getHwBrightness() );
}
