/*
 * SystemTime.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Basics.h"
#include "Peripherals/Clock.h"
#include "Peripherals/Oscillator.h"
#include "Peripherals/RealTimeCounter.h"

#include <Time/SystemTime.h>

uint16_t SystemTime::ticks( 0 );
int8_t SystemTime::ticksPerSecondAdjustment( 0 );

SystemTime::time_t SystemTime::now()
{
   convert_u actualTime;

   do
   {
      CriticalSection doNotInterrupt;
      if ( RealTimeCounter::getOverflowFlag() )
      {
         ticks++;
         RealTimeCounter::clearOverflowFlag();
      }
      actualTime.word[0] = RealTimeCounter::getCount();
   }
   while ( RealTimeCounter::getOverflowFlag() != 0 );

   actualTime.word[1] = ticks;
   return actualTime.dword;
}

SystemTime::time_t SystemTime::since( const SystemTime::time_t& lastTime )
{
   time_t elapsedTime, timestamp = now();

   // was there an overun
   if ( lastTime > timestamp )
   {
      elapsedTime = static_cast<time_t>( -1 ) - lastTime + timestamp;

      // it is not very common that the overrun was more than MAX_TIME/2 ago
      // assume in that case, that the lastTime is a timestamp in the (near) future
      if ( elapsedTime > static_cast<time_t>( -1 ) / 2 )
      {
         // the timestamp 'lastTime' was not elapsed
         elapsedTime = 0;
      }
   }
   else
   {
      elapsedTime = ( timestamp - lastTime );
   }

   return elapsedTime;
}

void SystemTime::waitMs( uint16_t ms )
{
   time_t timestamp = now();
   while ( since( timestamp ) < ms )
   {
   }
}

void SystemTime::set( SystemTime::time_t value )
{
   RTC_PRESCALER_t presc = RealTimeCounter::getPrescaler();
   RealTimeCounter::setPrescaler( RTC_PRESCALER_OFF_gc );
   RealTimeCounter::setCount( value );
   ticks = value >> 16;
   RealTimeCounter::setPrescaler( presc );
}

#ifdef SUPPORT_CALENDAR

#include <Time/Calender.h>

SIGNAL(RTC_COMP_vect)
{
   RealTimeCounter::setCompareValue(RealTimeCounter::getCompareValue() + SystemTime::S + SystemTime::ticksPerSecondAdjustment);
   Calender::now.addSecond();
}

#endif

void SystemTime::init( ClockSources cs, uint16_t frequency )
{
   CLK_RTCSRC_t rtcSource;
   RTC_PRESCALER_t prescaler;

   RealTimeCounter::setPrescaler( RTC_PRESCALER_OFF_gc );
   Clock::disableRTCClockSource();

   if ( ( cs == RTCSRC_EXTCLK ) && ( frequency == 8192 ) )
   {
      rtcSource = CLK_RTCSRC_EXTCLK_gc;
      prescaler = RTC_PRESCALER_DIV8_gc;
   }
   else
   {
      // Set internal 1kHz oscillator as clock source for RTC.
      rtcSource = CLK_RTCSRC_RCOSC_gc;
      prescaler = RTC_PRESCALER_DIV1_gc;
   }

   Clock::enableRTCClockSource( rtcSource );
   RealTimeCounter::init( 0xFFFF, RealTimeCounter::getCount(), RealTimeCounter::getCount() + 1024, prescaler );
#ifdef SUPPORT_CALENDAR
   RealTimeCounter::clearCompareFlag();
   RealTimeCounter::setCompareIntLevel( RTC_COMPINTLVL_HI_gc );

#endif
}

