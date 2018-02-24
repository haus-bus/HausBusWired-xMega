/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: Xmega192A3
        Configuration   : debug
        Model Element	: DigitalFrequencyLockedLoops
   //!	Generated Date	: Tue, 24, Jun 2014
        File Path	: Xmega192A3/debug/Peripherals/DigitalFrequencyLockedLoops.cpp
 *********************************************************************/

// ## auto_generated
#include "DigitalFrequencyLockedLoops.h"
// ## package Peripherals

// ## class DigitalFrequencyLockedLoops
DigitalFrequencyLockedLoops& DigitalFrequencyLockedLoops::instance( bool for32MHz )
{
   // #[ operation instance(bool)
   if ( for32MHz )
   {
      return *reinterpret_cast<DigitalFrequencyLockedLoops*> ( &DFLLRC32M );
   }

   return *reinterpret_cast<DigitalFrequencyLockedLoops*> ( &DFLLRC2M );
   // #]
}

/*********************************************************************
        File Path	: Xmega192A3/debug/Peripherals/DigitalFrequencyLockedLoops.cpp
*********************************************************************/
