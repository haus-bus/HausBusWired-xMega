/*
 * TimerCounterBase.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_TimerCounterBase_H
#define Peripherals_TimerCounterBase_H

#include "Peripherals/Peripherals.h"

class TimerCounterBase
{
   public:

      typedef struct TCBase_struct
      {
         register8_t CTRLA; /* Control  Register A */
         register8_t CTRLB; /* Control Register B */
         register8_t CTRLC; /* Control register C */
         register8_t CTRLD; /* Control Register D */
         register8_t CTRLE; /* Control Register E */
         register8_t reserved_0x05;
         register8_t INTCTRLA; /* Interrupt Control Register A */
         register8_t INTCTRLB; /* Interrupt Control Register B */
         register8_t CTRLFCLR; /* Control Register F Clear */
         register8_t CTRLFSET; /* Control Register F Set */
         register8_t CTRLGCLR; /* Control Register G Clear */
         register8_t CTRLGSET; /* Control Register G Set */
         register8_t INTFLAGS; /* Interrupt Flag Register */
         register8_t reserved_0x0D;
         register8_t reserved_0x0E;
         register8_t TEMP; /* Temporary Register For 16-bit Access */
         register8_t reserved_0x10;
         register8_t reserved_0x11;
         register8_t reserved_0x12;
         register8_t reserved_0x13;
         register8_t reserved_0x14;
         register8_t reserved_0x15;
         register8_t reserved_0x16;
         register8_t reserved_0x17;
         register8_t reserved_0x18;
         register8_t reserved_0x19;
         register8_t reserved_0x1A;
         register8_t reserved_0x1B;
         register8_t reserved_0x1C;
         register8_t reserved_0x1D;
         register8_t reserved_0x1E;
         register8_t reserved_0x1F;
         _WORDREGISTER( CNT ); /* Count */
         register8_t reserved_0x22;
         register8_t reserved_0x23;
         register8_t reserved_0x24;
         register8_t reserved_0x25;
         _WORDREGISTER( PER ); /* Period */
         _WORDREGISTER( CCA ); /* Compare or Capture A */
         _WORDREGISTER( CCB ); /* Compare or Capture B */
      } TCBase_t;

      ////    Constructors and destructors    ////

   protected:

      inline TimerCounterBase()
      {
      }

      ////    Operations    ////

   public:

      inline void clearCCAFlag()
      {
         baseReg.INTFLAGS = TC0_CCAIF_bm;
      }

      inline void clearCCBFlag()
      {
         baseReg.INTFLAGS = TC0_CCBIF_bm;
      }

      inline void clearErrorFlag()
      {
         baseReg.INTFLAGS = TC0_ERRIF_bm;
      }

      inline void clearOverflowFlag()
      {
         baseReg.INTFLAGS = TC0_OVFIF_bm;
      }

      inline void configClockSource( TC_CLKSEL_t clockSelection )
      {
         baseReg.CTRLA = ( baseReg.CTRLA & ~TC0_CLKSEL_gm ) | clockSelection;
      }

      inline void configEventAction( TC_EVSEL_t eventSource, TC_EVACT_t eventAction )
      {
         baseReg.CTRLD = ( baseReg.CTRLD & ~( TC0_EVSEL_gm | TC0_EVACT_gm ) )
                         | eventSource
                         | eventAction;
      }

      inline void configInputCapture( TC_EVSEL_t eventSource )
      {
         baseReg.CTRLD = ( baseReg.CTRLD & ~( TC0_EVSEL_gm | TC0_EVACT_gm ) )
                         | eventSource
                         | TC_EVACT_CAPT_gc;
      }

      inline void configWGM( TC_WGMODE_t wgm )
      {
         baseReg.CTRLB = ( baseReg.CTRLB & ~TC0_WGMODE_gm ) | wgm;
      }

      inline void disableEventDelay()
      {
         baseReg.CTRLD &= ~TC0_EVDLY_bm;
      }

      inline void enableEventDelay()
      {
         baseReg.CTRLD |= TC0_EVDLY_bm;
      }

      inline void forceUpdate()
      {
         baseReg.CTRLFSET = TC_CMD_UPDATE_gc;
      }

      inline uint8_t getCCAFlag()
      {
         return baseReg.INTFLAGS & TC0_CCAIF_bm;
      }

      inline uint8_t getCCBFlag()
      {
         return baseReg.INTFLAGS & TC0_CCBIF_bm;
      }

      inline uint16_t getCaptureA()
      {
         return baseReg.CCA;
      }

      inline uint16_t getCaptureB()
      {
         return baseReg.CCB;
      }

      inline uint8_t getErrorFlag()
      {
         return baseReg.INTFLAGS & TC0_ERRIF_bm;
      }

      inline uint8_t getOverflowFlag()
      {
         return baseReg.INTFLAGS & TC0_OVFIF_bm;
      }

      inline uint16_t getPeriod()
      {
         return baseReg.PER;
      }

      inline uint8_t isRunning()
      {
         return baseReg.CTRLA;
      }

      inline void lockCompareUpdate()
      {
         baseReg.CTRLFSET = TC0_LUPD_bm;
      }

      inline void reset()
      {
         // TC must be turned off before a Reset command.
         baseReg.CTRLA = ( baseReg.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_OFF_gc;

         // Issue Reset command.
         baseReg.CTRLFSET = TC_CMD_RESET_gc;
      }

      inline void restart()
      {
         baseReg.CTRLFSET = TC_CMD_RESTART_gc;
      }

      inline void setCCAIntLevel( TC_CCAINTLVL_t intLevel )
      {
         baseReg.INTCTRLB = ( baseReg.INTCTRLB & ~TC0_CCAINTLVL_gm ) | intLevel;
      }

      inline void setCCBIntLevel( TC_CCBINTLVL_t intLevel )
      {
         baseReg.INTCTRLB = ( baseReg.INTCTRLB & ~TC0_CCBINTLVL_gm ) | intLevel;
      }

      inline void setCount( uint16_t value )
      {
         baseReg.CNT = value;
      }

      inline void setErrorIntLevel( TC_ERRINTLVL_t intLevel )
      {
         baseReg.INTCTRLA = ( baseReg.INTCTRLA & ~TC0_ERRINTLVL_gm ) | intLevel;
      }

      inline void setOverflowIntLevel( TC_OVFINTLVL_t intLevel )
      {
         baseReg.INTCTRLA = ( baseReg.INTCTRLA & ~TC0_OVFINTLVL_gm ) | intLevel;
      }

      inline void setPeriod( uint16_t value )
      {
         baseReg.PER = value;
      }

      inline void unlockCompareUpdate()
      {
         baseReg.CTRLFCLR = TC0_LUPD_bm;
      }

      ////    Attributes    ////

   protected:

      TCBase_t baseReg;
};

#endif
