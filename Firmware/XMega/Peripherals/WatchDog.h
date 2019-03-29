/*
 * WatchDog.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_WatchDog_H
#define Peripherals_WatchDog_H

#include "Peripherals.h"

class WatchDog
{
   public:

      enum Timeout {
         _8MS = WDT_PER_8CLK_gc,
         _16MS = WDT_PER_16CLK_gc,
         _32MS = WDT_PER_32CLK_gc,
         _64MS = WDT_PER_64CLK_gc,
         _125MS = WDT_PER_128CLK_gc,
         _250MS = WDT_PER_256CLK_gc,
         _500MS = WDT_PER_512CLK_gc,
         _1S = WDT_PER_1KCLK_gc,
         _2S = WDT_PER_2KCLK_gc,
         _4S = WDT_PER_4KCLK_gc,
         _8S = WDT_PER_8KCLK_gc,
      };

      ////    Operations    ////

      ///*! \brief This function lock the entire clock system configuration.
      // *
      // *  This will lock the configuration until the next reset, or until the
      // *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
      // *  and switches to internal 2MHz RC oscillator.
      // */
      inline static void disable()
      {
         CCPWrite( &WDT.CTRL, ( WDT.CTRL & ~WDT_ENABLE_bm ) | WDT_CEN_bm );
      }

      ///*! \brief This function lock the entire clock system configuration.
      // *
      // *  This will lock the configuration until the next reset, or until the
      // *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
      // *  and switches to internal 2MHz RC oscillator.
      // */
      inline static void enable( Timeout timeout )
      {
         CCPWrite( &WDT.CTRL, WDT_ENABLE_bm | WDT_CEN_bm | timeout );
         // Wait for WD to synchronize with new settings.
         while ( isSyncBusy() )
         {
         }
      }

      ///*! \brief This function lock the entire clock system configuration.
      // *
      // *  This will lock the configuration until the next reset, or until the
      // *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
      // *  and switches to internal 2MHz RC oscillator.
      // */
      inline static void reset()
      {
         asm ( "wdr" );
      }

   private:

      ///*! \brief This function lock the entire clock system configuration.
      // *
      // *  This will lock the configuration until the next reset, or until the
      // *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
      // *  and switches to internal 2MHz RC oscillator.
      // */
      inline static bool isSyncBusy()
      {
         return ( WDT.STATUS & WDT_SYNCBUSY_bm );
      }
};

#endif
