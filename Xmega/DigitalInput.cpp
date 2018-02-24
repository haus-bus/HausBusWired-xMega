/*
 * DigitalInput.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "DigitalInput.h"

uint8_t DigitalInput::isSet() const
{
   return getIoPort().get() & getPin();
}

uint16_t DigitalInput::getPulseWidth( bool level, uint16_t timeout ) const
{
   uint8_t* port = ( (uint8_t*) &getIoPort() ) + 8;
   uint8_t pin = getPin();

   timeout *= COUNT_DELAY_BIT_US( 1 );

   CriticalSection doNotInterrupt;
   uint16_t remainingTimeStart = delayBit( timeout, port, pin, level );
   uint16_t remainingTimeEnd = delayBit( remainingTimeStart, port, pin, !level );

   if ( remainingTimeEnd == 0 )
   {
      // timeout
      return 0;
   }

   return remainingTimeStart - remainingTimeEnd;
}
