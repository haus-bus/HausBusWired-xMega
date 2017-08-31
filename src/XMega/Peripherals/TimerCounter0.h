/*
 * TimerCounter0.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_TimerCounter0_H
#define Peripherals_TimerCounter0_H

#include "TimerCounterBase.h"

class TimerCounter0 : public TimerCounterBase
{
   public:

      // #[ type TC0_t
      typedef struct TC0_struct
      {
         _WORDREGISTER( CCC ); /* Compare or Capture C */
         _WORDREGISTER( CCD ); /* Compare or Capture D */
         register8_t reserved_0x30;
         register8_t reserved_0x31;
         register8_t reserved_0x32;
         register8_t reserved_0x33;
         register8_t reserved_0x34;
         register8_t reserved_0x35;
         _WORDREGISTER( PERBUF ); /* Period Buffer */
         _WORDREGISTER( CCABUF ); /* Compare Or Capture A Buffer */
         _WORDREGISTER( CCBBUF ); /* Compare Or Capture B Buffer */
         _WORDREGISTER( CCCBUF ); /* Compare Or Capture C Buffer */
         _WORDREGISTER( CCDBUF ); /* Compare Or Capture D Buffer */
      } TC0_t;
      // #]

      ////    Operations    ////

      inline void clearCCCFlag()
      {
         baseReg.INTFLAGS = TC0_CCCIF_bm;
      }

      inline void clearCCDFlag()
      {
         baseReg.INTFLAGS = TC0_CCDIF_bm;
      }

      inline void disableChannels( uint8_t disableMask )
      {
         // Make sure only CCxEN bits are set in disableMask.
         disableMask &= ( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm );

         // Disable channels.
         baseReg.CTRLB &= ~disableMask;
      }

      inline void enableChannels( uint8_t enableMask )
      {
         // Make sure only CCxEN bits are set in enableMask.
         enableMask &= ( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm );

         // Enable channels.
         baseReg.CTRLB |= enableMask;
      }

      inline uint8_t getCCCFlag()
      {
         return baseReg.INTFLAGS & TC0_CCCIF_bm;
      }

      inline uint8_t getCCDFlag()
      {
         return baseReg.INTFLAGS & TC0_CCDIF_bm;
      }

      inline uint16_t getCaptureC()
      {
         return reg.CCC;
      }

      inline uint16_t getCaptureD()
      {
         return reg.CCD;
      }

      static TimerCounter0& instance( uint8_t portNumber );

      inline void setCCCIntLevel( TC_CCCINTLVL_t intLevel )
      {
         baseReg.INTCTRLB = ( baseReg.INTCTRLB & ~TC0_CCCINTLVL_gm ) | intLevel;
      }

      inline void setCCDIntLevel( TC_CCDINTLVL_t intLevel )
      {
         baseReg.INTCTRLB = ( baseReg.INTCTRLB & ~TC0_CCDINTLVL_gm ) | intLevel;
      }

      inline void setCompareA( uint16_t value )
      {
         reg.CCABUF = value;
      }

      inline void setCompareB( uint16_t value )
      {
         reg.CCBBUF = value;
      }

      inline void setCompareC( uint16_t value )
      {
         reg.CCCBUF = value;
      }

      inline void setCompareD( uint16_t value )
      {
         reg.CCDBUF = value;
      }

      inline void setPeriodBuffered( uint16_t value )
      {
         reg.PERBUF = value;
      }

      ////    Attributes    ////

   protected:

      TC0_t reg;        // ## attribute reg
};

#endif
