/*
 * Dht.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Dht.h"
#include <PortPin.h>

const uint8_t Dht::debugLevel( DEBUG_LEVEL_OFF );

Dht::Temperature::Temperature( uint8_t instanceNumber )
{
   setId( ( ClassId::TEMPERATURE << 8 ) | instanceNumber );
}

bool Dht::Temperature::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      if ( inStartUp() )
      {
         setConfiguration( ConfigurationManager::getConfiguration<EepromConfiguration>( id ) );
         if ( configuration )
         {
            SET_STATE_L1( RUNNING );
         }
         else
         {
            terminate();
            ErrorMessage::notifyOutOfMemory( id );
            return true;
         }
      }
      setSleepTime( NO_WAKE_UP );
   }
   else if ( event.isEvMessage() )
   {
      return handleRequest( event.isEvMessage()->getMessage() );
   }
   return false;
}

Dht::Dht( uint8_t instanceNumber, PortPin portPin ) :
   hardware( portPin ), itsTemperature( instanceNumber )
{
   setId( ( ClassId::HUMIDITY << 8 ) | instanceNumber );
}

void Dht::handleRunning()
{
   uint8_t rawData[sizeof( Data )];
   uint8_t error = hardware.read( rawData );
   if ( error )
   {
      if ( errorCounter++ > MAX_ERRORS )
      {
         Response event( getId() );
         event.setErrorCode( error );
         event.queue();

         Status status;
         status.value = 127;
         status.centiValue = 99;
         notifyNewValue( status );
         itsTemperature.notifyNewValue( status );
         SET_STATE_L1( IDLE );
      }
   }
   else
   {
      uint16_t help = ( ( rawData[4] << 8 ) + rawData[3] );
      Status status;
      status.value = help / 10;
      status.centiValue = 0;
      notifyNewValue( status );

      help = ( ( ( rawData[2] & 0x7F ) << 8 ) + rawData[1] );
      status.value = help / 10;
      status.centiValue = ( help % 10 ) * 10;
      itsTemperature.notifyNewValue( status );

      errorCounter = 0;
   }
   setSleepTime( SystemTime::S* configuration->reportTimeBase );
}

bool Dht::notifyEvent( const Event& event )
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

void Dht::run()
{
   if ( inStartUp() )
   {
      setConfiguration( ConfigurationManager::getConfiguration<EepromConfiguration>( id ) );
      if ( configuration )
      {
         SET_STATE_L1( RUNNING );
      }
      else
      {
         terminate();
         ErrorMessage::notifyOutOfMemory( id );
         return;
      }
      setSleepTime( SystemTime::S );
   }
   else if ( inIdle() )
   {
      setSleepTime( NO_WAKE_UP );
   }
   else if ( inRunning() )
   {
      handleRunning();
   }

}

Dht22* Dht::getHardware() const
{
   return (Dht22*) &hardware;
}

Dht::Temperature* Dht::getItsTemperature() const
{
   return (Dht::Temperature*) &itsTemperature;
}

