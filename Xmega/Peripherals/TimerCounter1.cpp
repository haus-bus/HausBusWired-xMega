/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: Xmega192A3
        Configuration   : debug
        Model Element	: TimerCounter1
   //!	Generated Date	: Tue, 24, Jun 2014
        File Path	: Xmega192A3/debug/Peripherals/TimerCounter1.cpp
 *********************************************************************/

// ## auto_generated
#include "Peripherals/TimerCounter1.h"
// ## package Peripherals

// ## class TimerCounter1
TimerCounter1& TimerCounter1::instance( uint8_t portNumber )
{
   // #[ operation instance(uint8_t)
   switch ( portNumber )
   {
    #ifdef TCC1
      case PortC:
         return *reinterpret_cast<TimerCounter1*> ( &TCC1 );
    #endif

    #ifdef TCD1
      case PortD:
         return *reinterpret_cast<TimerCounter1*> ( &TCD1 );
    #endif

    #ifdef TCE1
      case PortE:
         return *reinterpret_cast<TimerCounter1*> ( &TCE1 );
    #endif
   }

   return *(TimerCounter1*)0;
   // #]
}

/*********************************************************************
        File Path	: Xmega192A3/debug/Peripherals/TimerCounter1.cpp
*********************************************************************/
