/*
 * DigitalFrequencyLockedLoops.cpp
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#include "DigitalFrequencyLockedLoops.h"

DigitalFrequencyLockedLoops& DigitalFrequencyLockedLoops::instance( bool for32MHz )
{
   if ( for32MHz )
   {
      return *reinterpret_cast<DigitalFrequencyLockedLoops*> ( &DFLLRC32M );
   }

   return *reinterpret_cast<DigitalFrequencyLockedLoops*> ( &DFLLRC2M );
}


