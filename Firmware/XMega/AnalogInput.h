/*
 * AnalogInput.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Basics_AnalogInput_H
#define Basics_AnalogInput_H

#include "DigitalInput.h"
#include "Peripherals/Adc.h"

class AnalogInput : public DigitalInput
{
   public:

      ////    Constructors and destructors    ////

      inline AnalogInput( uint8_t _portNumber, uint8_t _pinNumber ) :
         DigitalInput( _portNumber, _pinNumber )
      {
      }

      ////    Operations    ////

   protected:

      ////    Additional operations    ////

   public:

      inline uint16_t getValue()
      {
         Adc& adc = Adc::instance( portNumber );
         adc.enable();

         Adc::Channel& channel = adc.getChannel( 0 );

         channel.setupInput( ADC_CH_INPUTMODE_SINGLEENDED_gc, pinNumber, 0,
                             ADC_CH_GAIN_1X_gc );
         channel.startConversion();
         channel.waitForConversionFinished();
         adc.disable();
         return channel.getResult();
      }

      ////    Attributes    ////

   protected:

};

#endif

