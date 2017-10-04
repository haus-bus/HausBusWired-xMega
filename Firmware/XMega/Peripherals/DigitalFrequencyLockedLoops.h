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

      ///*! \brief This function disables the automatic calibration of the internal oscillator.
      // *
      // */
      inline void disableAutoCalibration()
      {
         reg.CTRL &= ~DFLL_ENABLE_bm;
      }

      static DigitalFrequencyLockedLoops& instance( bool for32MHz );

      ////    Attributes    ////

   protected:

      DFLL_t reg;       // ## attribute reg
};

#endif
