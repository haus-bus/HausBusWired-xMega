/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: Xmega192A3
        Configuration   : debug
        Model Element	: IrDecoder
   //!	Generated Date	: Tue, 24, Jun 2014
        File Path	: Xmega192A3/debug/HwProtocols/IrDecoder.cpp
 *********************************************************************/

// ## auto_generated
#include "IrDecoder.h"
// ## package HwProtocols

// ## class IrDecoder
const uint8_t IrDecoder::debugLevel( DEBUG_LEVEL_OFF );

// ## class IrDecoder::RC5

// ## class IrDecoder::NEC

IrDecoder::IrDecoder() : irDetected( 0 ), startBitDetected( 0 ), waitForStartSpace( 0 ), repetitionFrame( 0 ), lastTime( 0xFFFF )
{
   // #[ operation IrDecoder()
   // #]
}

bool IrDecoder::getData( IrDecoder::Data& data )
{
   // #[ operation getData(Data)
   bool newData = irDetected;
   data = lastData;

   if ( newData )
   {
      irDetected = 0;
      startBitDetected = 0;
   }

   return newData;

   // #]
}

void IrDecoder::notifyEdge( uint8_t bit, uint16_t time )
{
   // #[ operation notifyEdge(uint8_t,uint16_t)
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
   // #]
}

/*********************************************************************
        File Path	: Xmega192A3/debug/HwProtocols/IrDecoder.cpp
*********************************************************************/
