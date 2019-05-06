/*
 * SystemTime.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include <Basics.h>
#include <Peripherals/Clock.h>
#include <Peripherals/Oscillator.h>
#include <Peripherals/RealTimeCounter.h>

#include <Time/SystemTime.h>


void SystemTime::init( ClockSources cs, uint16_t frequency )
{
   Clock::enableRTCClockSource( CLK_RTCSRC_RCOSC_gc );
   RealTimeCounter::init( 0xFFFF, RealTimeCounter::getCount(), 0, RTC_PRESCALER_DIV1_gc );
}

SystemTime::time_t SystemTime::now()
{
   return RealTimeCounter::getCount();

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
   RealTimeCounter::setPrescaler( presc );
}

