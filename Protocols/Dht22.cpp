/*
 * Dht22.cpp
 *
 *  Created on: 09.05.2015
 *      Author: viktor.pankraz
 */

#include <Protocols/Dht22.h>

#define getId() FSTR( "Dht22::" )

const uint8_t Dht22::debugLevel( DEBUG_LEVEL_OFF );



Dht22::Dht22( PortPin pin ) :
   portPin( pin )
{
   portPin.enablePullup();
}


uint8_t Dht22::read( uint8_t* data )
{
   DEBUG_H1( FSTR( "read" ) );
   if ( !waitForIdle() )
   {
      DEBUG_M1( FSTR( "BUS_HUNG" ) );
      return BUS_HUNG;
   }

   reset();

   uint8_t* port = ( (uint8_t*) &portPin.getIoPort() ) + 8;
   uint8_t pin = portPin.getPin();

   CriticalSection doNotInterrupt;
   // test();

   Errors error = waitForAck();
   if ( error )
   {
      return error;
   }

   uint8_t i = BYTE_COUNT;
   while ( i-- )
   {
      for ( uint8_t j = 0; j < 8; j++ )
      {
         data[i] <<= 1;
         // wait for bit change, record timing
         // the lower phase length is not really specified, just >50us
         uint16_t remainingTime = delayBit( COUNT_DELAY_BIT_US( 100 ), port, pin,
                                            1 );
         if ( !remainingTime )
         {
            // we are way out of timing spec
            DEBUG_L4( FSTR( "SYNC_TIMEOUT " ), i, ' ', j );
            return SYNC_TIMEOUT;
         }

         remainingTime = delayBit( COUNT_DELAY_BIT_US( 80 ), port, pin, 0 );
         if ( !remainingTime )
         {
            // we are way out of timing spec
            DEBUG_L4( FSTR( "DATA_TIMEOUT " ), i, ' ', j );
            return DATA_TIMEOUT;
         }

         if ( remainingTime < COUNT_DELAY_BIT_US( 40 ) )
         {
            data[i] |= 1;
         }
      }
   }
   DEBUG_M4( FSTR( "Data: " ), data[4], '*', data[3] );
   DEBUG_L4( '*', data[2], '*', data[1] );

   // checksum
   if ( data[0] != (uint8_t) ( data[1] + data[2] + data[3] + data[4] ) )
   {
      DEBUG_M1( FSTR( "CHECKSUM_ERROR" ) );
      return CHECKSUM_ERROR;
   }
   DEBUG_M1( FSTR( "OK" ) );

   return OK;
}
