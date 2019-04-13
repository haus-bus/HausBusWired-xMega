/*
 * MonitoredInput.cpp
 *
 *  Created on: 09.05.2016
 *      Author: viktor.pankraz
 */

#include <util/delay.h>
#include <Protocols/MonitoredInputHw.h>


uint16_t MonitoredInputHw::getResponseTime( uint16_t timeout )
{
   IoPort& ioPort = portPin.getIoPort();
   uint8_t* port = ( (uint8_t*) &portPin.getIoPort() ) + 8;
   uint8_t pin = portPin.getPin();

   uint16_t remaining;

   {
      CriticalSection doNotInterrupt;
      ioPort.setPins( pin );
      ioPort.setPinsAsOutput( pin );

      remaining = delayBit( COUNT_DELAY_BIT_US( 50 ), port, pin, 1 );
      if ( remaining )
      {
         _delay_us( 50 );
         ioPort.setPinsAsInput( pin );
         remaining = delayBit( COUNT_DELAY_BIT_US( 1000 ), port, pin, 0 );
      }
   }
   return remaining;
}

