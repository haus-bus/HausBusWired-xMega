/*
 * IrDecoder.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include <Protocols/IrDecoder.h>

const uint8_t IrDecoder::debugLevel( DEBUG_LEVEL_OFF );



IrDecoder::IrDecoder() : irDetected( 0 ), startBitDetected( 0 ), waitForStartSpace( 0 ), repetitionFrame( 0 ), lastTime( 0xFFFF )
{
}

bool IrDecoder::getData( IrDecoder::Data& data )
{
   bool newData = irDetected;
   data = lastData;

   if ( newData )
   {
      irDetected = 0;
      startBitDetected = 0;
   }

   return newData;

}

void IrDecoder::notifyEdge( uint8_t bit, uint16_t time )
{
   if ( !irDetected )
   {
      if ( !startBitDetected )                        // start bit detected?
      {                                              // no...
         if ( bit && time )
         {
            if ( ( lastTime == 0 ) || ( lastTime > MIN_IDLE_TIME ) )
            {
               DEBUG_M1( 'S' );
               DEBUG_L3( '\\', time, '/' );
               startBitDetected = 1;
               waitForStartSpace = 1;

               if ( lastTime && ( lastTime < MAX_REPETITION_DELAY ) )
               {
                  repetitionFrame = 1;
               }
               else
               {
                  repetitionFrame = 0;
               }
            }
         }
      }
      else
      {
         if ( waitForStartSpace )     // we have received start bit...
         {
            // receiving first data pulse!
            DEBUG_L2( time, '\\' );
            if ( RC5::isStartBit( lastTime, time, parameter ) )
            {
            }
            else if ( NEC::isStartBit( lastTime, time ) )
            {
            }
            else
            {
               ERROR_1( FSTR( "unknown IR protocol" ) );
               startBitDetected = 0; // wait for another start bit...
            }
            waitForStartSpace = 0;
         }
         else
         {
            if ( bit )
            {
               DEBUG_L2( time, '/' );
            }
            else
            {
               DEBUG_L2( time, '\\' );
            }

            if ( parameter.protocol == RC5_PROTOCOL )
            {
               irDetected = RC5::notifyEdge( bit, lastTime, time, parameter );
            }

            if ( !parameter.bitPosition )
            {
               ERROR_1( FSTR( "IR timing" ) );
               startBitDetected = 0;
            }
            if ( irDetected )
            {
               if ( repetitionFrame )
               {
                  if ( lastData.repetitionFrameNumber < 0xFF )
                  {
                     lastData.repetitionFrameNumber++;
                  }
               }
               else
               {
                  lastData.repetitionFrameNumber = 1;
               }
               lastData.address = parameter.address;
               lastData.command = parameter.command;
               DEBUG_M2( FSTR( "address: " ), parameter.address );
               DEBUG_M2( FSTR( "command: " ), parameter.command );
            }

         }
      }
   }
   lastTime = time;
}