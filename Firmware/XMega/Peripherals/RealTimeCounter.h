/*
 * RealTimeCounter.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_RealTimeCounter_H
#define Peripherals_RealTimeCounter_H

#include "Peripherals.h"

class RealTimeCounter
{
   public:

      /* This is the interrupt vector declaration. Copy it to your
       *  program code if you want to use it. Leave commented if it's not
       *  used to avoid overhead when compiling.
       *
         ISR(RTC_COMP_vect)
         {
         }

         ISR(RTC_OVF_vect)
         {
         }
       *
       */

      ////    Operations    ////
      inline static void clearCompareFlag()
      {
         RTC.INTFLAGS = RTC_COMPIF_bm;
      }

      inline static void clearOverflowFlag()
      {
         RTC.INTFLAGS = RTC_OVFIF_bm;
      }

      inline static uint8_t getCompareFlag()
      {
         return RTC.INTFLAGS & RTC_COMPIF_bm;
      }

      inline static uint16_t getCompareValue()
      {
         return RTC.COMP;
      }

      inline static uint16_t getCount()
      {
         return RTC.CNT;
      }

      inline static uint8_t getOverflowFlag()
      {
         return RTC.INTFLAGS & RTC_OVFIF_bm;
      }

      inline static uint16_t getPeriod()
      {
         return RTC.PER;
      }

      inline static RTC_PRESCALER_t getPrescaler()
      {
         return (RTC_PRESCALER_t)( RTC.CTRL & RTC_PRESCALER_gm );
      }

      inline static void init( uint16_t period, uint16_t count,
                               uint16_t compareValue, RTC_PRESCALER_t prescaler )
      {
         setPeriod( period );
         setCount( count );
         setCompareValue( compareValue );
         setPrescaler( prescaler );
      }

      inline static uint8_t isBusy()
      {
         return RTC.STATUS & RTC_SYNCBUSY_bm;
      }

      inline static void setAlarm( uint16_t alarmTimeout )
      {
         if ( alarmTimeout <= 2 )
         {
            // Due to synchronization between RTC clock and the system clock domains, there is a latency of
            // two RTC clock cycles from updating the register until this has an effect.
            alarmTimeout = 3;
         }

         // Calculate compare time.
         uint16_t compareValue = getCount() + alarmTimeout;

         // Wrap on period.
         if ( compareValue >= getPeriod() )
         {
            compareValue -= getPeriod();
         }

         // Add the timeout value to get the absolute time of the alarm.
         setCompareValue( compareValue );
      }

      inline static void setCompareIntLevel( RTC_COMPINTLVL_t intLevel )
      {
         RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_COMPINTLVL_gm ) | intLevel;
      }

      inline static void setCompareValue( uint16_t value )
      {
         while ( isBusy() )
         {
         }
         RTC.COMP = value;
      }

      inline static void setCount( uint16_t value )
      {
         while ( isBusy() )
         {
         }
         RTC.CNT = value;
      }

      inline static void setIntLevels( RTC_OVFINTLVL_t ovfIntLevel,
                                       RTC_COMPINTLVL_t compIntLevel )
      {
         RTC.INTCTRL = ( RTC.INTCTRL & ~( RTC_COMPINTLVL_gm | RTC_OVFINTLVL_gm ) )
                       | ovfIntLevel | compIntLevel;
      }

      inline static void setOverflowIntLevel( RTC_OVFINTLVL_t intLevel )
      {
         RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_OVFINTLVL_gm ) | intLevel;
      }

      inline static void setPeriod( uint16_t value )
      {
         while ( isBusy() )
         {
         }
         RTC.PER = value;
      }

      inline static void setPrescaler( RTC_PRESCALER_t prescaler )
      {
         while ( isBusy() )
         {
         }
         RTC.CTRL = ( RTC.CTRL & ~RTC_PRESCALER_gm ) | prescaler;
      }

};

#endif
