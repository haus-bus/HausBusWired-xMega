/*
 * TimerCounter.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "TimerCounter.h"

TimerCounter& TimerCounter::instance( uint8_t portNumber, uint8_t type )
{
   if ( type == 0 )
   {
      switch ( portNumber )
      {
         #ifdef TCC0
         case PortC:
            return *reinterpret_cast<TimerCounter*> ( &TCC0 );
         #endif

         #ifdef TCD0
         case PortD:
            return *reinterpret_cast<TimerCounter*> ( &TCD0 );
         #endif

         #ifdef TCE0
         case PortE:
            return *reinterpret_cast<TimerCounter*> ( &TCE0 );
         #endif

         #ifdef TCF0
         case PortF:
            return *reinterpret_cast<TimerCounter*> ( &TCF0 );
         #endif
      }
   }
   else if ( type == 1 )
   {
      switch ( portNumber )
      {
         #ifdef TCC1
         case PortC:
            return *reinterpret_cast<TimerCounter*> ( &TCC1 );
         #endif

         #ifdef TCD1
         case PortD:
            return *reinterpret_cast<TimerCounter*> ( &TCD1 );
         #endif

         #ifdef TCE1
         case PortE:
            return *reinterpret_cast<TimerCounter*> ( &TCE1 );
         #endif
      }
   }
   WARN_3( FSTR( "Requested Timer not exists! port" ), (uint8_t)( 'A' + portNumber ), type );
   return *(TimerCounter*)0;
}

