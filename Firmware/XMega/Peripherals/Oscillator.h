/*
 * Oscillator.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Oscillator_H
#define Peripherals_Oscillator_H

#include "Peripherals.h"

class Oscillator
{
   public:

      ////    Operations    ////

      ///*! \brief This function configures the external oscillator.
      // *
      // *  \note Note that the oscillator cannot be used as a main system clock
      // *        source without being enabled and stable first. Check the ready flag
      // *        before using the clock. The macro CLKSYS_IsReady( _oscSel )
      // *        can be used to check this.
      // *
      // *  \param  freqRange          Frequency range for high-frequency crystal, does
      // *                             not apply for external clock or 32kHz crystals.
      // *  \param  lowPower32kHz      True of high-quality watch crystals are used and
      // *                             low-power oscillator is desired.
      // *  \param  xoscModeSelection  Combined selection of oscillator type (or
      // *                             external clock) and startup times.
      // */
      inline static void configExtOscillator( OSC_FRQRANGE_t freqRange,
                                              bool lowPower32kHz,
                                              OSC_XOSCSEL_t xoscModeSelection )
      {
         OSC.XOSCCTRL = (uint8_t) freqRange | ( lowPower32kHz ? OSC_X32KLPM_bm : 0 )
                        | xoscModeSelection;
      }

      ///*! \brief This function configures the internal high-frequency PLL.
      // *
      // *  Configuration of the internal high-frequency PLL to the correct
      // *  values. It is used to define the input of the PLL and the factor of
      // *  multiplication of the input clock source.
      // *
      // *  \note Note that the oscillator cannot be used as a main system clock
      // *        source without being enabled and stable first. Check the ready flag
      // *        before using the clock. The macro CLKSYS_IsReady( _oscSel )
      // *        can be used to check this.
      // *
      // *  \param  clockSource Reference clock source for the PLL,
      // *                      must be above 0.4MHz.
      // *  \param  factor      PLL multiplication factor, must be
      // *                      from 1 to 31, inclusive.
      // */
      inline static void configPLL( OSC_PLLSRC_t clockSource, uint8_t factor )
      {
         factor &= OSC_PLLFAC_gm;
         OSC.PLLCTRL = (uint8_t) clockSource | ( factor << OSC_PLLFAC_gp );
      }

      ///*! \brief This function disables the selected oscillator.
      // *
      // *  This function will disable the selected oscillator if possible.
      // *  If it is currently used as a main system clock source, hardware will
      // *  disregard the disable attempt, and this function will return zero.
      // *  If it fails, change to another main system clock source and try again.
      // *
      // *  \param oscSel  Bitmask of selected clock. Can be one of the following
      // *                 OSC_RC2MEN_bm, OSC_RC32MEN_bm, OSC_RC32KEN_bm,
      // *                 OSC_XOSCEN_bm, OSC_PLLEN_bm.
      // *
      // *  \return  Non-zero if oscillator was disabled successfully.
      // */
      inline static uint8_t disable( uint8_t selection )
      {
         OSC.CTRL &= ~selection;
         uint8_t clkEnabled = OSC.CTRL & selection;
         return clkEnabled;
      }

      ///*! \brief This function enables the selected oscillator.
      // *
      // *  \note Note that the oscillator cannot be used as a main system clock
      // *        source without being enabled and stable first. Check the ready flag
      // *        before using the clock. The function isReady( selection )
      // *        can be used to check this.
      // *
      // *  \param  _selection Bitmask of selected clock. Can be one of the following
      // *                  bmRC2MEN, bmRC32MEN, bmRC32KEN, bmXOSCEN, bmPLLEN.
      // */
      inline static void enable( uint8_t selection )
      {
         OSC.CTRL |= selection;
      }

      ///*! \brief This function enables automatic calibration of the selected internal
      // *         oscillator.
      // *
      // *  Either the internal 32kHz RC oscillator or an external 32kHz
      // *  crystal can be used as a calibration reference. The user must make sure
      // *  that the selected reference is ready and running.
      // *
      // *  \param  clkSource    Clock source to calibrate, either OSC_RC2MCREF_bm or
      // *                       OSC_RC32MCREF_bm.
      // *  \param  extReference True if external crystal should be used as reference.
      // */
      inline static void enableAutoCalibration( uint8_t clkSource,
                                                bool extReference )
      {
// OSC.DFLLCTRL = ( OSC.DFLLCTRL & ~clkSource )
// | ( extReference ? clkSource : 0 );
//
// if (clkSource == OSC_RC2MCREF_bm)
// {
// DFLLRC2M.CTRL |= DFLL_ENABLE_bm;
// }
// else if (clkSource == OSC_RC32MCREF_bm)
// {
// DFLLRC32M.CTRL |= DFLL_ENABLE_bm;
// }
////#]
      }

      ///*! \brief This function enables the External Oscillator Failure Detection
      // *         (XOSCFD) feature.
      // *
      // *  The feature will stay enabled until next reset. Note that the
      // *  XOSCFD _will_ issue the XOSCF Non-maskable Interrupt (NMI) regardless of
      // *  any interrupt priorities and settings. Therefore, make sure that a handler
      // *  is implemented for the XOSCF NMI when you enable it.
      // */
      inline static void enableExtOscillatorFailtureDetection()
      {
         CCPWrite( &OSC.XOSCFAIL, ( OSC_XOSCFDIF_bm | OSC_XOSCFDEN_bm ) );
      }

      ///*! \brief This function check if selected oscillator is ready.
      // *
      // *  This function will return non-zero if it is running, regardless if it is
      // *  used as a main clock source or not.
      // *
      // *  \param  _selection Bitmask of selected clock. Can be one of the following
      // *                  bmRC2MEN, bmRC32MEN, bmRC32KEN, bmXOSCEN, bmPLLEN.
      // *
      // *  \return  Non-zero if oscillator is ready and running.
      // */
      inline static uint8_t isReady( uint8_t selection )
      {
         return OSC.STATUS & selection;
      }

      inline static void waitUntilOscillatorIsReady( uint8_t selection )
      {
         while ( isReady( selection ) != selection )
         {
         }
      }

      inline static void setRC32KCalibrationValue( uint8_t value )
      {
         OSC.RC32KCAL = value;
      }

      inline static uint8_t getRC32KCalibrationValue()
      {
         return OSC.RC32KCAL;
      }

};

#endif
