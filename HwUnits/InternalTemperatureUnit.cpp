/*
 * AnalogInputUnit.cpp
 *
 *  Created on: 22.10.2015
 *      Author: Viktor Pankraz
 */

#include <ErrorMessage.h>
#include <InternalTemperature.h>
#include "InternalTemperatureUnit.h"

const uint8_t InternalTemperatureUnit::debugLevel( DEBUG_LEVEL_OFF );

InternalTemperatureUnit::InternalTemperatureUnit()
{
   Object::setId( ( ClassId::TEMPERATURE << 8 ) | 0xFF );
}

bool InternalTemperatureUnit::notifyEvent( const Event& event )
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

void InternalTemperatureUnit::run()
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
   }

   int16_t centiCelsius = InternalTemperature::getValue();

   Status status;
   status.value = ( centiCelsius / 100 );
   status.centiValue = ( abs( centiCelsius % 100 ) );
   notifyNewValue( status );
   setSleepTime( SystemTime::MIN );

}
