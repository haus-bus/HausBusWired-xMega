/*
 * DigitalFrequencyLockedLoops.h
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_DigitalFrequencyLockedLoops_H
#define Peripherals_DigitalFrequencyLockedLoops_H

#include "Peripherals.h"

class DigitalFrequencyLockedLoops
{
   public:

      ////    Constructors and destructors    ////

   protected:

      inline DigitalFrequencyLockedLoops()
      {
      }

      ////    Operations    ////

   public:

      // This function enables the automatic calibration of the internal oscillator.
      inline void enableAutoCalibration()
      {
         reg.CTRL |= DFLL_ENABLE_bm;
      }

      // This function disables the automatic calibration of the internal oscillator.
      inline void disableAutoCalibration()
      {
         reg.CTRL &= ~DFLL_ENABLE_bm;
      }

      static inline DigitalFrequencyLockedLoops& instance( bool for32MHz )
      {
         if ( for32MHz )
         {
            return *reinterpret_cast<DigitalFrequencyLockedLoops*> ( &DFLLRC32M );
         }

         return *reinterpret_cast<DigitalFrequencyLockedLoops*> ( &DFLLRC2M );
      }

      ////    Attributes    ////

   protected:

      DFLL_t reg;
};

#endif
