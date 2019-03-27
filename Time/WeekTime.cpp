/*
 * WeekTime.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "WeekTime.h"
#include "Calender.h"
#include <CriticalSection.h>

WeekTime WeekTime::sunRise( WEEK_DAY_MASK | 0x800 );

WeekTime WeekTime::sunSet( WEEK_DAY_MASK | 0x1400 );

WeekTime::WeekTime( uint16_t time )
{
   set( time & ( WEEK_DAY_MASK | HOURS_MASK | MINUTES_MASK ) );
}

bool WeekTime::isElapsed( uint16_t weekTime )
{
   uint16_t _now = ( get() | getTimeMask( weekTime ) );
   return ( weekTime <= _now );
}

int16_t WeekTime::distanceToNow()
{
   CriticalSection doNotInterrupt;

   uint16_t nowMinutes = Calender::now.getDayOfWeek() * MAX_HOURS * MAX_MINUTES
                         + Calender::now.getHour() * MAX_MINUTES + Calender::now.getMinute();
   uint16_t currentMinutes = weekDay * MAX_HOURS * MAX_MINUTES
                             + hour * MAX_MINUTES + minute;

   return nowMinutes - currentMinutes;
}

uint16_t WeekTime::getTimeMask( uint16_t weekTime )
{
   uint16_t mask = 0;

   if ( ( weekTime & WEEK_DAY_MASK ) == WEEK_DAY_MASK )
   {
      mask |= WEEK_DAY_MASK;
   }
   if ( ( weekTime & HOURS_MASK ) == HOURS_MASK )
   {
      mask |= HOURS_MASK;
   }
   if ( ( weekTime & MINUTES_MASK ) == MINUTES_MASK )
   {
      mask |= MINUTES_MASK;
   }
   return mask;
}

bool WeekTime::isInRange( WeekTime start, WeekTime end )
{
   if ( start < end )
   {
      return ( *this >= start && *this <= end );
   }
   else if ( start > end )
   {
      return !( *this > end && *this < start );
   }
   return start == *this;
}

bool WeekTime::operator <( WeekTime time )
{
   uint16_t thisMasked = ( get() | time.getMask() );
   return ( thisMasked < time.get() );
}

bool WeekTime::operator >( WeekTime time )
{
   uint16_t thisMasked = ( get() | time.getMask() );
   return ( thisMasked > time.get() );
}

bool WeekTime::operator ==( WeekTime time )
{
   uint16_t thisMasked = ( get() | time.getMask() );
   return ( thisMasked == time.get() );
}
