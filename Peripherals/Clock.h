/*
 * Clock.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Clock_H
#define Peripherals_Clock_H

#include "Peripherals.h"

class Clock
{
   public:

      ////    Operations    ////

      ///*! \brief This function changes the prescaler configuration.
      // *
      // *  Change the configuration of the three system clock
      // *  prescaler is one single operation. The user must make sure that
      // *  the main CPU clock does not exceed recommended limits.
      // *
      // *  \param  PSAfactor   Prescaler A division factor, OFF or 2 to 512 in
      // *                      powers of two.
      // *  \param  PSBCfactor  Prescaler B and C division factor, in the combination
      // *                      of (1,1), (1,2), (4,1) or (2,2).
      // */

      inline static void configPrescalers( CLK_PSADIV_t PSAfactor,
                                           CLK_PSBCDIV_t PSBCfactor )
      {
         CCPWrite( &CLK.PSCTRL, PSAfactor | PSBCfactor );
      }

      ///*! \brief This function disables routing of clock signals to the Real-Time
      // *         Counter (RTC).
      // *
      // *  Disabling the RTC saves power if the RTC is not in use.
      // */
      inline static void disableRTCClockSource()
      {
         CLK.RTCCTRL &= ~CLK_RTCEN_bm;
      }

      ///*! \brief This function selects a Real-Time Counter clock source.
      // *
      // *  Selects the clock source for use by the Real-Time Counter (RTC)
      // *  and enables clock signal routing to the RTC module.
      // *
      // *  \param  clockSource  Clock source to use for the RTC.
      // */
      inline static void enableRTCClockSource( CLK_RTCSRC_t clockSource )
      {
         CLK.RTCCTRL = ( CLK.RTCCTRL & ~CLK_RTCSRC_gm ) | clockSource | CLK_RTCEN_bm;
      }

      ///*! \brief This function lock the entire clock system configuration.
      // *
      // *  This will lock the configuration until the next reset, or until the
      // *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
      // *  and switches to internal 2MHz RC oscillator.
      // */
      inline static void lockConfiguration()
      {
         CCPWrite( &CLK.LOCK, CLK_LOCK_bm );
      }

      ///*! \brief This function selects the main system clock source.
      // *
      // *  Hardware will disregard any attempts to select a clock source that is not
      // *  enabled or not stable. If the change fails, make sure the source is ready
      // *  and running and try again.
      // *
      // *  \param  clockSource  Clock source to use as input for the system clock
      // *                       prescaler block.
      // *
      // *  \return  Non-zero if change was successful.
      // */
      inline static uint8_t selectMainClockSource( CLK_SCLKSEL_t clockSource )
      {
         CCPWrite( &CLK.CTRL, ( CLK.CTRL & ~CLK_SCLKSEL_gm ) | clockSource );
         return ( CLK.CTRL & clockSource );
      }

      inline static void enableModule( Port port, uint8_t id )
      {
         CriticalSection critical;
         *( (uint8_t*) &PR.PRGEN + port + 1 ) &= ~id;
      }

      inline static void disableModule( Port port, uint8_t id )
      {
         CriticalSection critical;
         *( (uint8_t*) &PR.PRGEN + port + 1 ) |= id;
      }

      inline static void enableGeneralModule( uint8_t id )
      {
         CriticalSection critical;
         PR.PRGEN &= ~id;
      }

      inline static void disableGeneralModule( uint8_t id )
      {
         CriticalSection critical;
         PR.PRGEN |= id;
      }
};

#endif
