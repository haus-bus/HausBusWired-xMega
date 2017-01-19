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
  Object::setId(
      (ClassId::TEMPERATURE << 8) | 0xFF );

  configuration = HwConfiguration::getSensorUnitConfiguration( id );
  if ( configuration )
  {
// TODO
  }
  else
  {
    terminate();
    ErrorMessage( getId(), ErrorMessage::CONFIGURATION_OUT_OF_MEMORY );
  }
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
    SET_STATE_L1( RUNNING );
  }

  int16_t centiCelsius = InternalTemperature::getValue();

  Status status;
  status.value = ( centiCelsius / 100 );
  status.centiValue = ( abs(centiCelsius % 100 ) );
  notifyNewValue( status );
  setSleepTime( SystemTime::MIN );

}
