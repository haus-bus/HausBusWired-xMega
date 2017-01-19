/*
 * AnalogInputUnit.cpp
 *
 *  Created on: 22.10.2015
 *      Author: Viktor Pankraz
 */

#include <ErrorMessage.h>
#include "AnalogInputUnit.h"

const uint8_t AnalogInputUnit::debugLevel( DEBUG_LEVEL_OFF );

AnalogInputUnit::AnalogInputUnit( PortPin portPin ) :
    analogInput( portPin.getPortNumber(), portPin.getPinNumber() )
{
  Object::setId(
      (ClassId::ANALOG_INPUT << 8) | ((portPin.getPortNumber() + 1) << 4)
          | (portPin.getPinNumber() + 1) );

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

bool AnalogInputUnit::notifyEvent( const Event& event )
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

void AnalogInputUnit::run()
{
  if ( inStartUp() )
  {
    SET_STATE_L1( RUNNING );
  }
  uint16_t value = analogInput.getValue();

  DEBUG_H2( FSTR(".value: 0x"), value);

  Status status;
  status.value = ( value / 1000 );
  status.centiValue = ( (value % 1000) / 10 );
  notifyNewValue( status );
  setSleepTime( SystemTime::MIN );
}
