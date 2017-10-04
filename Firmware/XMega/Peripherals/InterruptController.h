/*
 * InterruptController.h
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_InterruptController_H
#define Peripherals_InterruptController_H

#include "Peripherals.h"

class InterruptController
{
   ////    Operations    ////

   public:

      inline static void enableAllInterruptLevel()
      {
         PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
      }

      inline static void disableRoundRobinScheduling()
      {
         PMIC.CTRL &= ~PMIC_RREN_bm;
      }

      inline static void enableRoundRobinScheduling()
      {
         PMIC.CTRL |= PMIC_RREN_bm;
      }

      inline static void selectAppInterruptSection()
      {
         CCPWrite( &PMIC.CTRL, PMIC.CTRL & ~PMIC_IVSEL_bm );
      }

      inline static void selectBootInterruptSection()
      {
         CCPWrite( &PMIC.CTRL, PMIC.CTRL | PMIC_IVSEL_bm );
      }
};

#endif
