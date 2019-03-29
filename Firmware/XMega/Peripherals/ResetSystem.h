/*
 * ResetSystem.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_ResetSystem_H
#define Peripherals_ResetSystem_H

#include "Peripherals.h"

class ResetSystem
{
   ////    Operations    ////

   public:

      ///*! \brief This function lock the entire clock system configuration.
      // *
      // *  This will lock the configuration until the next reset, or until the
      // *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
      // *  and switches to internal 2MHz RC oscillator.
      // */

      inline static void clearSources( uint8_t sources = RST_SDRF_bm | RST_SRF_bm | RST_PDIRF_bm | RST_WDRF_bm | RST_BORF_bm | RST_EXTRF_bm | RST_PORF_bm )
      {
         RST.STATUS = sources;
      }

      ///*! \brief This function lock the entire clock system configuration.
      // *
      // *  This will lock the configuration until the next reset, or until the
      // *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
      // *  and switches to internal 2MHz RC oscillator.
      // */
      inline static uint8_t getSources()
      {
         return RST.STATUS;
      }

      inline static uint8_t isBrownOutReset()
      {
         return getSources() & RST_BORF_bm;
      }

      inline static uint8_t isDebuggerReset()
      {
         return getSources() & RST_PDIRF_bm;
      }

      inline static uint8_t isExternalReset()
      {
         return getSources() & RST_EXTRF_bm;
      }

      inline static uint8_t isPowerOnReset()
      {
         return getSources() & RST_PORF_bm;
      }

      inline static uint8_t isSoftwareReset()
      {
         return getSources() & RST_SRF_bm;
      }

      inline static uint8_t isSpikeDetectorReset()
      {
         return getSources() & RST_SDRF_bm;
      }

      inline static uint8_t isWatchDogReset()
      {
         return getSources() & RST_WDRF_bm;
      }

      ///*! \brief This function lock the entire clock system configuration.
      // *
      // *  This will lock the configuration until the next reset, or until the
      // *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
      // *  and switches to internal 2MHz RC oscillator.
      // */
      inline static void reset()
      {
         CCPWrite( &RST.CTRL, RST_SWRST_bm );
      }
};

#endif
