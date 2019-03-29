/*
 * InternalTemperature.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include "InternalTemperature.h"
#include "Peripherals/Adc.h"

#include <util/delay.h>

const uint8_t InternalTemperature::debugLevel( DEBUG_LEVEL_OFF );

#define getId() FSTR( "IntTemp::" )


int16_t InternalTemperature::getValue()
{
   DEBUG_H1( FSTR( "getValue()" ) );

   Adc& adc = Adc::instance( PortA );
   adc.enable();
   adc.enableInternalInput( ADC_TEMPREF_bm );
   Adc::Channel& channel = adc.getChannel( 0 );

   channel.setupInput( ADC_CH_INPUTMODE_INTERNAL_gc, ADC_CH_MUXINT_TEMP_gc, 0,
                       ADC_CH_GAIN_1X_gc );
   channel.startConversion();
   channel.waitForConversionFinished();
   adc.disable();

   int16_t value = channel.getResult();
   uint16_t cal = Adc::getTemperatureCalibration();
   DEBUG_M2( FSTR( "cal: 0x" ), cal );

   // reference is ADC reading at 358 K, scaled by 100 to get units of 0.01C
   uint16_t adcPerCentiKelvin = ( (uint16_t)( 273 + 85 ) * 100 ) / cal;

   DEBUG_M2( FSTR( "adcPerCentiKelvin: 0x" ), adcPerCentiKelvin );
   DEBUG_M2( FSTR( "adcResult: 0x" ), (uint16_t)value );

   ADCA.CTRLA = 0; // turn ADC off
   ADCA.REFCTRL = 0; // turn temperature sensor off

   value *= adcPerCentiKelvin;

   value -= 27300;

   DEBUG_M2( FSTR( "centiCelsius: 0x" ), (uint16_t)value );
   return value;
}

