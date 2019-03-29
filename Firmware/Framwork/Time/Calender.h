/*
 * Calender.h
 *
 *  Created on: 25.04.2016
 *      Author: viktor.pankraz
 */

#ifndef TIME_CALENDER_H_
#define TIME_CALENDER_H_

#include "WeekTime.h"
#include "EventDrivenUnit.h"
#include "EventPkg/EventPkg.h"

class EventDrivenUnit;

class Calender
{
   public:

      // ! Unix epoch year
      static const uint16_t EPOCH_YEAR = 1970;

      static EventDrivenUnit* minuteListener;

      static Calender now;

      Calender( uint8_t _second = 0, uint8_t _minute = 0, uint8_t _hour = 0,
                uint8_t _date = 0, uint8_t _month = 0, uint16_t _year = 2000,
                uint8_t _dayofweek = 5 );

      bool isValid();

      uint32_t toTimestamp();

      uint32_t toFatSystemTimestamp();

      Calender getDifferenceTo( Calender& cal );

      void addSecond();

      inline uint16_t getYear()
      {
         return year;
      }

      inline uint8_t getMonth()
      {
         return month;
      }

      inline uint8_t getDate()
      {
         return date;
      }

      inline uint8_t getHour()
      {
         return hour;
      }

      inline uint8_t getMinute()
      {
         return minute;
      }

      inline uint8_t getSecond()
      {
         return second;
      }

      inline uint8_t getDayOfWeek()
      {
         return dayofweek;
      }

      inline static WeekTime getCurrentWeekTime()
      {
         return WeekTime( now.getMinute(), now.getHour(), now.getDayOfWeek() );
      }

      inline void setWeekTime( WeekTime wTime )
      {
         second = 0;
         minute = wTime.getMinute();
         hour = wTime.getHour();
         dayofweek = wTime.getWeekDay();
         evTime( minuteListener ).queue();
      }

      static Calender fromTimeStamp( uint32_t timestamp, uint16_t startYear
                                        = EPOCH_YEAR );

   private:

      static uint8_t isLeapYear( uint16_t year );

      static inline uint16_t sizeofYear( uint16_t year )
      {
         if ( isLeapYear( year ) )
         {
            return 366;
         }
         return 365;
      }

      inline void addYear()
      {
         if ( year < 2105 )
         {
            year++;
         }
      }

      inline void addMonth()
      {
         month++;
         if ( month == 12 )
         {
            month = 0;
            addYear();
         }
      }

      void addDay();

      inline void addHour()
      {
         hour++;
         if ( hour == 24 )
         {
            hour = 0;
            addDay();
         }
      }

      inline void addMinute()
      {
         minute++;
         if ( minute == 60 )
         {
            minute = 0;
            addHour();
         }
         evTime( minuteListener ).queue();
      }

      uint8_t second;    // !< 0-59
      uint8_t minute;    // !< 0-59
      uint8_t hour;      // !< 0-23
      uint8_t date;      // !< 0-30 \note First day of month is 0, not 1.
      uint8_t month;     // !< 0 January - 11 December
      uint16_t year;     // !< 1970-2105
      uint8_t dayofweek; // !< 0 Sunday  - 6 Saturday

};

#endif /* TIME_CALENDER_H_ */
