/*
 * Dimmer.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Dimmer.h"

const uint8_t Dimmer::debugLevel( DEBUG_LEVEL_OFF | DEBUG_STATE_L3 );

void Dimmer::Response::setBrightness( uint16_t brightness )
{
   if ( brightness )
   {
      controlFrame.setDataLength(
         sizeof( getResponse() ) + sizeof( getParameter().brightness ) );
      setResponse( EVENT_ON );
      getParameter().brightness = (uint8_t) ( brightness / 10 );
   }
   else
   {
      controlFrame.setDataLength( sizeof( getResponse() ) );
      setResponse( EVENT_OFF );
   }

}

Dimmer::Response::Parameter& Dimmer::Response::setConfiguration()
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().configuration ) );
   setResponse( CONFIGURATION );
   return getParameter();
}

void Dimmer::Response::setDirection( uint8_t direction )
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().direction ) );
   setResponse( EVENT_START );
   getParameter().direction = direction;
}

void Dimmer::Response::setStatus( uint16_t brightness )
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().brightness ) );
   setResponse( STATUS );
   getParameter().brightness = (uint8_t) ( brightness / 10 );

}

Dimmer::Dimmer( uint8_t _id, IDimmerHw* _hardware ) :
   brightness( 0 ), direction( TO_LIGHT ), duration( 0 ),
   subState( NO_COMMAND ), targetBrightness( 0 )
{
   hardware = NULL;
   configuration = NULL;
   setId( ( ClassId::DIMMER << 8 ) | _id );
   hardware = _hardware;
}

bool Dimmer::notifyEvent( const Event& event )
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

void Dimmer::run()
{
   if ( inStartUp() )
   {
      setDirection( TO_DARK );
      setBrightness( 0 );
      setTargetBrightness( 0 );
      configureHw();
      SET_STATE_L1( IDLE );
   }

   if ( inIdle() || inFailture() )
   {
      SET_STATE_L2( NO_COMMAND );
      setSleepTime( NO_WAKE_UP );
      if ( inIdle() )
      {
         hardware->suspend( IDimmerHw::CHECK_ENABLED );
         uint8_t errorCode = hardware->hasError();
         if ( errorCode )
         {
            notifyError( errorCode );
            SET_STATE_L1( FAILTURE );
         }
      }

      if ( duration != 0 )
      {
         if ( --duration != 0 )
         {
            setSleepTime( TIME_BASE );
            return;
         }
         Command::SetBrightness setBrightness;
         setBrightness.duration = 0;
         setBrightness.brightness = 0;
         cmdSetBrightness( setBrightness );
      }
   }
   else if ( inRunning() )
   {
      setSleepTime( 10 );
      handleRunningState();
   }

}

void Dimmer::cmdSetBrightness( Dimmer::Command::SetBrightness& parameter,
                               uint8_t changeTime )
{
   if ( parameter.brightness > IDimmerHw::MAX_BRIGHTNESS )
   {
      parameter.brightness = IDimmerHw::MAX_BRIGHTNESS;
   }
   if ( parameter.brightness > configuration->dimmingRangeEnd )
   {
      parameter.brightness = configuration->dimmingRangeEnd;
   }
   if ( parameter.brightness != ( brightness / 10 ) )
   {
      setTargetBrightness( parameter.brightness * 10 );
      setDirection( ( targetBrightness > brightness ) ? TO_LIGHT : TO_DARK );
      direction *= getStepWidth( changeTime );
   }
   SET_STATE_L1( RUNNING );
   SET_STATE_L2( DIMMING );
   if ( targetBrightness )
   {
      hardware->suspend( IDimmerHw::DISABLE );
   }
   setSleepTime( 20 );
}

void Dimmer::cmdStart( const Dimmer::Command::Start& parameter )
{
   Command::SetBrightness setBrightness;
   setBrightness.duration = 0;
   if ( parameter.direction == TO_DARK )
   {
      setDirection( TO_LIGHT ); // set inverted direction because it will be toggeled
   }
   else if ( parameter.direction == TO_LIGHT )
   {
      setDirection( TO_DARK ); // set inverted direction because it will be toggeled
   }
   setBrightness.brightness = ( ( direction < 0 ) ? IDimmerHw::MAX_BRIGHTNESS : 0 );
   cmdSetBrightness( setBrightness, configuration->dimmingTime );
}

void Dimmer::configureHw()
{
   setConfiguration( ConfigurationManager::getConfiguration<EepromConfiguration>( id ) );

   notifyError( hardware->setMode( configuration->mode ) );
   hardware->off();

}

bool Dimmer::handleRequest( HACF* message )
{
   if ( !message->controlFrame.isCommand() )
   {
      return false;
   }

   HACF::ControlFrame& cf = message->controlFrame;
   Command* data = reinterpret_cast<Command*>( cf.getData() );

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
         response.setStatus( getBrightness() );
      }
      response.queue( getObject( message->header.getSenderId() ) );
   }

   if ( cf.isCommand( Command::SET_CONFIGURATION ) )
   {
      DEBUG_H1( FSTR( ".setConfiguration()" ) );
      configuration->set( data->parameter.setConfiguration );
      hardware->setMode( configuration->mode );
   }
   else
   {
      Response event( getId() );
      if ( cf.isCommand( Command::SET_BRIGHTNESS ) )
      {
         Command::SetBrightness& parameter = data->parameter.setBrightness;
         DEBUG_H5( FSTR( ".setBrightness(" ), parameter.brightness, ',',
                   parameter.duration, ')' );
         setDuration( parameter.duration );
         cmdSetBrightness( parameter );
         event.setDirection( ( getDirection() > 0 ) ? TO_LIGHT : TO_DARK );
         DEBUG_M2( FSTR( "brightness: " ), getBrightness() );
         DEBUG_M2( FSTR( "target    : " ), getTargetBrightness() );
         DEBUG_M2( FSTR( "duration  : " ), getDuration() );
      }
      else if ( cf.isCommand( Command::START ) )
      {
         DEBUG_H1( FSTR( ".start()" ) );
         setDuration( 0 );
         cmdStart( data->parameter.start );
         event.setDirection( ( getDirection() > 0 ) ? TO_LIGHT : TO_DARK );
      }
      else if ( cf.isCommand( Command::STOP ) )
      {
         DEBUG_H1( FSTR( ".stop()" ) );
         setDuration( 0 );
         SET_STATE_L1( IDLE );
         event.setBrightness( getBrightness() );
      }
      else
      {
         return false;
      }
      event.queue();
   }
   return true;
}

void Dimmer::handleRunningState()
{
   if ( getSubState() == DIMMING )
   {
      if ( ( direction > 0 ) && ( brightness < targetBrightness ) )
      {
         if ( brightness < getDimmingRangeStart() )
         {
            setBrightness( getDimmingRangeStart() );
         }
         else
         {
            brightness += direction;
         }
         if ( ( brightness > targetBrightness )
            || ( brightness > getDimmingRangeEnd() ) )
         {
            setBrightness( targetBrightness );
         }
      }
      else if ( ( direction < 0 ) && ( brightness > targetBrightness ) )
      {
         if ( brightness > getDimmingRangeEnd() )
         {
            setBrightness( getDimmingRangeEnd() );
         }
         else
         {
            brightness += direction;
         }
         if ( ( brightness < targetBrightness )
            || ( brightness < getDimmingRangeStart() ) )
         {
            setBrightness( targetBrightness );
         }
      }
      else
      {
         handleSuspend();
         SET_STATE_L2( NO_COMMAND );
         setBrightness( targetBrightness );
         SET_STATE_L1( IDLE );
         Response event( getId() );
         event.setBrightness( brightness );
         event.queue();
      }
   }
   else
   {
      handleSuspend();
      SET_STATE_L1( IDLE );
      return;
   }

   DEBUG_H2( FSTR( " level: " ), (uint16_t )brightness );
   hardware->on( brightness );
}

void Dimmer::handleSuspend()
{
   if ( !hardware->isOn() )
   {
      hardware->suspend( IDimmerHw::ENABLE );
      setSleepTime( 1000 );
   }
}

void Dimmer::notifyError( uint8_t errorCode )
{
   if ( errorCode )
   {
      Response event( getId() );
      event.setErrorCode( errorCode );
      event.queue();
   }
}

IDimmerHw* Dimmer::getHardware() const
{
   return hardware;
}

void Dimmer::setHardware( IDimmerHw* p_IDimmerHw )
{
   hardware = p_IDimmerHw;
}

