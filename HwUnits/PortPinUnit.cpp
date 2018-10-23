/*
 * PortPinUnit.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "PortPinUnit.h"
#include <PortPin.h>

const uint8_t PortPinUnit::debugLevel( DEBUG_LEVEL_OFF );

PortPinUnit::Response::Parameter& PortPinUnit::Response::setConfiguration()
{
   controlFrame.setDataLength( sizeof( getResponse() ) + sizeof( getParameter().configuration ) );
   setResponse( CONFIGURATION );
   return getParameter();
}

void PortPinUnit::Response::setEvent( uint8_t event )
{
   controlFrame.setDataLength( sizeof( getResponse() ) );
   setResponse( event );
}

void PortPinUnit::Response::setEventOn( uint16_t duration )
{
   controlFrame.setDataLength( sizeof( getResponse() ) + sizeof( getParameter().duration ) );
   setResponse( EVENT_ON );
   getParameter().duration = duration;
}

void PortPinUnit::Response::setStatus( uint8_t status )
{
   controlFrame.setDataLength( sizeof( getResponse() ) + sizeof( getParameter().status ) );
   setResponse( STATUS );
   getParameter().status = status;
}

PortPinUnit::PortPinUnit( PortPin _hardware ) :
   quantity( 0 ), onTime( 0 ), offTime( 0 ), duration( 0 ), feedback( 0 ),
   driveOnState( 0 ), driveOffState( 0 ),
   hardware( _hardware.getPortNumber(), _hardware.getPinNumber() )
{
   configuration = NULL;
   Object::setId( ( ClassId::DIGITAL_OUTPUT << 8 ) | ( ( _hardware.getPortNumber() + 1 ) << 4 )
                  | ( _hardware.getPinNumber() + 1 ) );
}

void PortPinUnit::cmdToggle( const PortPinUnit::Command::Toggle& parameter )
{
   setOnTime( parameter.onTime );
   setOffTime( parameter.offTime );
   setQuantity( parameter.quantity );
   setSleepTime( WAKE_UP );
}

void PortPinUnit::enableFeedback( bool enable )
{
   feedback = enable;
   updateHw();
}

bool PortPinUnit::handleRequest( HACF* message )
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
   }
   else
   {
      if ( cf.isCommand( Command::GET_CONFIGURATION )
         || cf.isCommand( Command::GET_STATUS ) )
      {
         Response response( getId(), *message );
         if ( cf.isCommand( Command::GET_CONFIGURATION ) )
         {
            DEBUG_H1( FSTR( ".getConfiguration()" ) );
            configuration->get( response.setConfiguration().configuration );
         }
         else
         {
            DEBUG_H1( FSTR( ".getStatus()" ) );
            response.setStatus( state.sub );
         }
         response.queue( getObject( message->header.getSenderId() ) );
      }
      else
      {
         Response response( getId() );
         if ( cf.isCommand( Command::OFF ) )
         {
            DEBUG_H1( FSTR( ".off()" ) );
            SET_STATE_L2( OFF );
            setDuration( 0 );
            setQuantity( 0 );
            response.setEvent( Response::EVENT_OFF );
            setSleepTime( NO_WAKE_UP );
         }
         else if ( cf.isCommand( Command::ON ) )
         {
            DEBUG_H1( FSTR( ".on()" ) );
            SET_STATE_L2( ON );
            setQuantity( 1 );
            setDuration( data->parameter.on.duration );
            setSleepTime( duration ? configuration->timeBase : NO_WAKE_UP );
            response.setEventOn( duration );
         }
         else if ( cf.isCommand( Command::TOGGLE ) )
         {
            DEBUG_H1( FSTR( ".toggle()" ) );
            cmdToggle( data->parameter.toggle );
            response.setEvent( Response::EVENT_TOGGLE );
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

bool PortPinUnit::notifyEvent( const Event& event )
{
   bool consumed = false;
   if ( event.isEvWakeup() )
   {
      bool hasNewEvent = run();
      if ( hasNewEvent )
      {
         Response event( getId() );
         event.setEvent(
            ( state.sub == ON ) ? Response::EVENT_ON : Response::EVENT_OFF );
         event.queue();
      }
   }
   else if ( event.isEvMessage() )
   {
      consumed = handleRequest( event.isEvMessage()->getMessage() );
   }

   updateHw();
   return consumed;
}

void PortPinUnit::updateConfiguration()
{
   Configuration::Options options = configuration->getOptions();
   hardware.setInverted( options.invert );
   setDriveOnState( options.driveOn );
   setDriveOffState( options.driveOff );
}

bool PortPinUnit::run()
{
   if ( inStartUp() )
   {
      setConfiguration( ConfigurationManager::getConfiguration<EepromConfiguration>( id ) );
      updateConfiguration();
      SET_STATE_L1( RUNNING );
      setSleepTime( NO_WAKE_UP );
   }
   else
   {
      uint16_t timeBase = configuration->timeBase;
      setSleepTime( timeBase );
      if ( duration != 0 )
      {
         if ( --duration != 0 )
         {
            return false;
         }
      }
      updateQuantity();

      if ( state.sub == ON )
      {
         SET_STATE_L2( OFF );
         setDuration( offTime );
      }
      else
      {
         SET_STATE_L2( ON );
         setDuration( onTime );
      }
      return getSleepTime() == NO_WAKE_UP;
   }
   return false;
}

void PortPinUnit::updateQuantity()
{
   if ( quantity != 0 )
   {
      if ( --quantity == 0 )
      {
         setSleepTime( NO_WAKE_UP );
      }
   }

}

void PortPinUnit::updateHw()
{
   if ( feedback || ( state.sub == ON ) )
   {
      if ( driveOnState )
      {
         hardware.configOutput();
         hardware.set();
      }
      else
      {
         hardware.configInput();
      }
   }
   else
   {
      if ( driveOffState )
      {
         hardware.configOutput();
         hardware.clear();
      }
      else
      {
         hardware.configInput();
      }
   }
}