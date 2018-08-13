/*
 * TimerCounterBase.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_TimerCounter_H
#define Peripherals_TimerCounter_H

#include "Peripherals.h"

class TimerCounter
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
      } TC01_t;

      enum Channel
      {
         A = 0,
         B = 1,
         C = 2,
         D = 3
      };

      ////    Constructors and destructors    ////

   protected:

      inline TimerCounter()
      {
      }

      ////    Operations    ////

   public:

      static TimerCounter& instance( uint8_t portNumber, uint8_t type );

      static inline Channel getChannelFromPinNumber( uint8_t pinNumber )
      {
         if ( pinNumber >= 4 )
         {
            pinNumber -= 4;
         }
         return (Channel)pinNumber;
      }

      inline void clearCCAFlag()
      {
         reg.INTFLAGS = TC0_CCAIF_bm;
      }

      inline void clearCCBFlag()
      {
         reg.INTFLAGS = TC0_CCBIF_bm;
      }

      inline void clearErrorFlag()
      {
         reg.INTFLAGS = TC0_ERRIF_bm;
      }

      inline void clearOverflowFlag()
      {
         reg.INTFLAGS = TC0_OVFIF_bm;
      }

      inline void configClockSource( TC_CLKSEL_t clockSelection )
      {
         reg.CTRLA = ( reg.CTRLA & ~TC0_CLKSEL_gm ) | clockSelection;
      }

      inline void configEventAction( TC_EVSEL_t eventSource, TC_EVACT_t eventAction )
      {
         reg.CTRLD = ( reg.CTRLD & ~( TC0_EVSEL_gm | TC0_EVACT_gm ) )
                     | eventSource
                     | eventAction;
      }

      inline void configInputCapture( TC_EVSEL_t eventSource )
      {
         reg.CTRLD = ( reg.CTRLD & ~( TC0_EVSEL_gm | TC0_EVACT_gm ) )
                     | eventSource
                     | TC_EVACT_CAPT_gc;
      }

      inline void configWGM( TC_WGMODE_t wgm )
      {
         reg.CTRLB = ( reg.CTRLB & ~TC0_WGMODE_gm ) | wgm;
      }

      inline void disableEventDelay()
      {
         reg.CTRLD &= ~TC0_EVDLY_bm;
      }

      inline void enableEventDelay()
      {
         reg.CTRLD |= TC0_EVDLY_bm;
      }

      inline void forceUpdate()
      {
         reg.CTRLFSET = TC_CMD_UPDATE_gc;
      }

      inline uint16_t getCount()
      {
         return reg.CNT;
      }

      inline uint8_t getCCAFlag()
      {
         return reg.INTFLAGS & TC0_CCAIF_bm;
      }

      inline uint8_t getCCBFlag()
      {
         return reg.INTFLAGS & TC0_CCBIF_bm;
      }

      inline uint16_t getCaptureA()
      {
         return reg.CCA;
      }

      inline uint16_t getCaptureB()
      {
         return reg.CCB;
      }

      inline uint8_t getErrorFlag()
      {
         return reg.INTFLAGS & TC0_ERRIF_bm;
      }

      inline uint8_t getOverflowFlag()
      {
         return reg.INTFLAGS & TC0_OVFIF_bm;
      }

      inline uint16_t getPeriod()
      {
         return reg.PER;
      }

      inline uint8_t isRunning()
      {
         return reg.CTRLA;
      }

      inline void lockCompareUpdate()
      {
         reg.CTRLFSET = TC0_LUPD_bm;
      }

      inline void reset()
      {
         // TC must be turned off before a Reset command.
         reg.CTRLA = ( reg.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_OFF_gc;

         // Issue Reset command.
         reg.CTRLFSET = TC_CMD_RESET_gc;
      }

      inline void restart()
      {
         reg.CTRLFSET = TC_CMD_RESTART_gc;
      }

      inline void setCCAIntLevel( TC_CCAINTLVL_t intLevel )
      {
         reg.INTCTRLB = ( reg.INTCTRLB & ~TC0_CCAINTLVL_gm ) | intLevel;
      }

      inline void setCCBIntLevel( TC_CCBINTLVL_t intLevel )
      {
         reg.INTCTRLB = ( reg.INTCTRLB & ~TC0_CCBINTLVL_gm ) | intLevel;
      }

      inline void setCount( uint16_t value )
      {
         reg.CNT = value;
      }

      inline void setErrorIntLevel( TC_ERRINTLVL_t intLevel )
      {
         reg.INTCTRLA = ( reg.INTCTRLA & ~TC0_ERRINTLVL_gm ) | intLevel;
      }

      inline void setOverflowIntLevel( TC_OVFINTLVL_t intLevel )
      {
         reg.INTCTRLA = ( reg.INTCTRLA & ~TC0_OVFINTLVL_gm ) | intLevel;
      }

      inline void setPeriod( uint16_t value )
      {
         reg.PER = value;
      }

      inline void unlockCompareUpdate()
      {
         reg.CTRLFCLR = TC0_LUPD_bm;
      }

      inline void clearCCFlag( Channel channel )
      {
         reg.INTFLAGS = TC0_CCAIF_bm << channel;
      }

      inline uint8_t getCCFlag( Channel channel )
      {
         return reg.INTFLAGS & ( TC0_CCAIF_bm << channel );
      }

      inline uint16_t getCapture( Channel channel )
      {
         register16_t* tmp = &reg.CCA;
         return tmp[channel];
      }

      inline void disableChannel( Channel channel )
      {
         reg.CTRLB &= ~( TC0_CCAEN_bm << channel );
      }

      inline void enableChannel( Channel channel )
      {
         reg.CTRLB |= ( TC0_CCAEN_bm << channel );
      }

      inline void setCompare( Channel channel, uint16_t value )
      {
         register16_t* tmp = &reg.CCA;
         tmp[channel] = value;
      }

      inline void setCCIntLevel( Channel channel, TC_CCCINTLVL_t intLevel )
      {
         reg.INTCTRLB = ( reg.INTCTRLB & ~( TC0_CCAINTLVL_gm << channel ) ) | intLevel;
      }

      inline void clearCCCFlag()
      {
         reg.INTFLAGS = TC0_CCCIF_bm;
      }

      inline void clearCCDFlag()
      {
         reg.INTFLAGS = TC0_CCDIF_bm;
      }

      inline void disableChannels( uint8_t disableMask )
      {
         // Make sure only CCxEN bits are set in disableMask.
         disableMask &= ( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm );

         // Disable channels.
         reg.CTRLB &= ~disableMask;
      }

      inline void enableChannels( uint8_t enableMask )
      {
         // Make sure only CCxEN bits are set in enableMask.
         enableMask &= ( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm );

         // Enable channels.
         reg.CTRLB |= enableMask;
      }

      inline uint8_t getCCCFlag()
      {
         return reg.INTFLAGS & TC0_CCCIF_bm;
      }

      inline uint8_t getCCDFlag()
      {
         return reg.INTFLAGS & TC0_CCDIF_bm;
      }

      inline uint16_t getCaptureC()
      {
         return reg.CCC;
      }

      inline uint16_t getCaptureD()
      {
         return reg.CCD;
      }

      inline void setCCCIntLevel( TC_CCCINTLVL_t intLevel )
      {
         reg.INTCTRLB = ( reg.INTCTRLB & ~TC0_CCCINTLVL_gm ) | intLevel;
      }

      inline void setCCDIntLevel( TC_CCDINTLVL_t intLevel )
      {
         reg.INTCTRLB = ( reg.INTCTRLB & ~TC0_CCDINTLVL_gm ) | intLevel;
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

      TC01_t reg;         // ## attribute baseReg
};

#endif
