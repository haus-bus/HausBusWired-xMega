/*
 * Dali.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include <Protocols/Dali.h>
#include <util/delay.h>

Dali::Dali( DigitalInput _digitalIn, DigitalOutput _digitalOut ) : digitalIn( _digitalIn ), digitalOut( _digitalOut )
{
}

bool Dali::isConnected()
{
   digitalOut.set();
   _delay_us( 10 );
   if ( digitalIn.isSet() )
   {
      _delay_ms( 10 );
      digitalOut.clear();
      if ( !digitalIn.isSet() )
      {
         return true;
      }
   }

   return false;
}

IStream::Status Dali::read( uint8_t* pData )
{
   *pData = 0;

   uint8_t* port = (uint8_t*)&digitalIn.getIoPort() + 8;
   uint8_t pin = digitalIn.getPin();

   // wait for the begin of the start bit, max 9.17msec
   uint16_t remainingTime = delayBit( COUNT_DELAY_BIT_US( 9170 ), port, pin, 0 );
   if ( !remainingTime )
   {
      return IStream::TIMEOUT;
   }

   // wait for rising edge of start bit
   remainingTime = delayBit( COUNT_DELAY_BIT_US( DALI_HALF_BIT_USEC ) * 1.5, port, pin, 1 );

   if ( ( remainingTime > COUNT_DELAY_BIT_US( DALI_HALF_BIT_USEC ) )
      || ( remainingTime == 0 ) )
   {
      // we are way out of timing spec
      return IStream::ABORTED;
   }

   // pin is high again, second half of the start bit
   uint8_t waitfor = 0;

   for ( int i = 0; i < 8; i++ )
   {
      // wait for bit change, record timing
      remainingTime = delayBit( COUNT_DELAY_BIT_US( DALI_HALF_BIT_USEC ) * 2.5, port, pin, waitfor );

      if ( ( remainingTime > COUNT_DELAY_BIT_US( DALI_HALF_BIT_USEC ) * 2 )
         || ( remainingTime == 0 ) )
      {
         // we are way out of timing spec
         return IStream::ABORTED;
      }
      else if ( remainingTime > COUNT_DELAY_BIT_US( DALI_HALF_BIT_USEC ) )
      {
         // bit-change at bit-border -> don't care
         waitfor = ( waitfor ? 0 : 1 );
         remainingTime = delayBit( COUNT_DELAY_BIT_US( DALI_HALF_BIT_USEC ) * 1.5, port, pin, waitfor );

         if ( ( remainingTime > COUNT_DELAY_BIT_US( DALI_HALF_BIT_USEC ) )
            || ( remainingTime == 0 ) )
         {
            // we are way out of timing spec
            return IStream::ABORTED;
         }
      }

      // bit-change at middle of bit time: data
      *pData |= waitfor;
      if ( i < 7 )
      {
         *pData <<= 1;
      }
      waitfor = ( waitfor ? 0 : 1 );
   }
   return IStream::SUCCESS;
}

IStream::Status Dali::write( uint8_t* pData )
{
   // we expect an inverter in the output stage:
   // atmega output normally low, dali bus normally high

   // DALI uses Manchester encoding:
   // rising edge at the half of the bit time means 1

   // with the inverter:
   // rising edge (SET) is 0
   // falling edge (CLEAR) is 1

   // start bit '1'
   digitalOut.set();      // PIN_SET(DALI_OUT);
   DALI_HALF_BIT_WAIT;
   digitalOut.clear();    // PIN_CLEAR(DALI_OUT);
   DALI_HALF_BIT_WAIT;

   // transmit the bytes
   for ( uint8_t i = 0; i < 2; i++ )
   {
      uint8_t b = pData[i];

      // most significant bit first
      for ( uint8_t j = 0; j < 8; j++, b <<= 1 )
      {
         // we are between two bits

         if ( b & 0x80 )
         {
            // bit is set
            digitalOut.set();   // PIN_SET(DALI_OUT);
            DALI_HALF_BIT_WAIT;
            digitalOut.clear(); // PIN_CLEAR(DALI_OUT);
            DALI_HALF_BIT_WAIT;
         }
         else
         {
            // bit is clear
            digitalOut.clear(); // PIN_CLEAR(DALI_OUT);
            DALI_HALF_BIT_WAIT;
            digitalOut.set();   // PIN_SET(DALI_OUT);
            DALI_HALF_BIT_WAIT;
         }
      }
   }

   // 2 stop bits always high
   digitalOut.clear();    // PIN_CLEAR(DALI_OUT);
   DALI_HALF_BIT_WAIT;
   DALI_HALF_BIT_WAIT;
   DALI_HALF_BIT_WAIT;
   DALI_HALF_BIT_WAIT;

   return IStream::SUCCESS;
}
