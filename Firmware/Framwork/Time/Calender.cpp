/*
 * Calender.cpp
 *
 *  Created on: 25.04.2016
 *      Author: viktor.pankraz
 */

#include "Calender.h"
#include <CriticalSection.h>

// ! Number of seconds in a day
#define SECS_PER_DAY 86400UL

// ! Number of seconds in an hour
#define SECS_PER_HOUR 3600UL

// ! Number of seconds in a minute
#define SECS_PER_MINUTE 60UL

// ! Number of days in a specified month. Index 1 for leap year, else 0.
const uint8_t daysPerMonth[2][12] = { { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
                                        30, 31 }, { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } };

Calender Calender::now;

EventDrivenUnit* Calender::minuteListener = NULL;

Calender::Calender( uint8_t _second, uint8_t _minute, uint8_t _hour,
                    uint8_t _date, uint8_t _month, uint16_t _year,
                    uint8_t _dayofweek ) :
   second( _second ), minute( _minute ), hour( _hour ), date( _date ),
   month( _month ), year( _year ), dayofweek( _dayofweek )
{
}

bool Calender::isValid()
{

   // Make sure time is valid
   if ( ( second >= 60 ) || ( minute >= 60 ) || ( hour >= 24 ) )
   {
      return false;
   }

   // Make sure month and date is valid
   if ( ( month >= 12 ) || ( date >= 31 ) )
   {
      return false;
   }

   // Make sure days in month are not more than it should be
   if ( date >= daysPerMonth[isLeapYear( year )][month] )
   {
      return false;
   }

   // Make sure year is not earlier than 1970 and before 2106
   if ( ( year < EPOCH_YEAR ) || ( year >= 2106 ) )
   {
      return false;
   }
   else
   {
      return true;
   }
}

uint8_t Calender::isLeapYear( uint16_t year )
{
   if ( !( ( year ) % 4 ) && ( ( ( year ) % 100 ) || !( ( year ) % 400 ) ) )
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

void Calender::addDay()
{
   date++;
   if ( date == daysPerMonth[isLeapYear( year )][month] )
   {
      date = 0;
      addMonth();
   }
   dayofweek++;
   if ( dayofweek == 7 )
   {
      dayofweek = 0;
   }
}

void Calender::addSecond()
{
   CriticalSection doNotInterrupt;
   second++;
   if ( second == 60 )
   {
      second = 0;
      addMinute();
   }
}

uint32_t Calender::toFatSystemTimestamp()
{
   uint32_t fattime = ( ( year - 1980 ) << ( 25 - 16 ) ) | ( ( month + 1 ) << ( 21 - 16 ) )
                      | ( ( date + 1 ) << ( 16 - 16 ) );
   fattime <<= 16;
   fattime |= ( hour << 11 ) | ( minute << 5 ) | ( second << 0 );

   return fattime;
}

uint32_t Calender::toTimestamp()
{

   // Make sure date is valid
   if ( !isValid() )
   {
      return 0;
   }

   uint32_t timestamp = 0;
   uint8_t date_month = month;
   uint16_t date_year = year;


   // Add number of seconds elapsed in current month
   timestamp += ( date * SECS_PER_DAY ) + ( hour * SECS_PER_HOUR )
                + ( minute * SECS_PER_MINUTE ) + second;

   while ( date_month != 0 )
   {
      date_month--;
      // Add number of seconds in months of current year
      timestamp += daysPerMonth[isLeapYear( date_year )][date_month]
                   * SECS_PER_DAY;
   }
   while ( date_year > EPOCH_YEAR )
   {
      date_year--;
      // Add number of seconds in all years since epoch year
      timestamp += isLeapYear( date_year ) * SECS_PER_DAY;
   }

   return timestamp;
}

Calender Calender::fromTimeStamp( uint32_t timestamp, uint16_t startYear )
{
   uint16_t year = startYear;
   uint8_t month = 0;
   uint32_t day_clock = timestamp % SECS_PER_DAY;
   uint32_t numberOfDays = timestamp / SECS_PER_DAY;
   uint8_t second = ( day_clock % SECS_PER_MINUTE );
   uint8_t minute = ( ( day_clock % SECS_PER_HOUR ) / SECS_PER_MINUTE );
   uint8_t hour = ( day_clock / SECS_PER_HOUR );
   uint8_t dayofweek = ( ( numberOfDays + 4 ) % 7 );

   while ( numberOfDays >= sizeofYear( year ) )
   {
      numberOfDays -= sizeofYear( year );
      year++;
   }

   while ( numberOfDays >= daysPerMonth[isLeapYear( year )][month] )
   {
      numberOfDays -= daysPerMonth[isLeapYear( year )][month];
      month++;
   }

   return Calender( second, minute, hour, numberOfDays, month, year, dayofweek );
}

Calender Calender::getDifferenceTo( Calender& cal )
{
   uint32_t difference = abs( toTimestamp() - cal.toTimestamp() );
   return fromTimeStamp( difference, 0 );
}
