/*
 * WeekTime.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Time_WeekTime_H
#define SwFramework_Time_WeekTime_H

#include "Time.h"

class WeekTime
{
   public:

      ////    type Constants    ////
      static const uint8_t MAX_MINUTES = 60;
      static const uint8_t MAX_HOURS = 24;
      static const uint8_t MAX_WEEK_DAYS = 7;

      static const uint16_t WEEK_DAY_MASK = 0xE000;
      static const uint16_t HOURS_MASK = 0x1F00;
      static const uint16_t MINUTES_MASK = 0x003F;

      ////    Constructors and destructors    ////

      WeekTime( uint16_t time = 0 );

      inline WeekTime( uint8_t _minute, uint8_t _hour, uint8_t _weekDay ) :
         minute( _minute ), hour( _hour ), weekDay( _weekDay )
      {
      }

      ////    Operations    ////

      bool isElapsed( uint16_t weekTime );

      int16_t distanceToNow();

      inline uint16_t get();

      inline int16_t getMinutesFromNow();

      static uint16_t getTimeMask( uint16_t weekTime );

      inline uint8_t isDay();

      bool isInRange( WeekTime start, WeekTime end );

      inline uint8_t isSunRise();

      inline uint8_t isSunSet();

      inline void set( uint16_t timestamp );

      ////    Additional operations    ////

      bool operator <( WeekTime time );

      bool operator >( WeekTime time );

      bool operator ==( WeekTime time );

      inline bool operator >=( WeekTime time )
      {
         return !( *this < time );
      }

      inline bool operator <=( WeekTime time )
      {
         return !( *this > time );
      }

      inline bool operator !=( WeekTime time )
      {
         return !( *this == time );
      }

      inline uint16_t getMask()
      {
         return WeekTime::getTimeMask( get() );
      }

      inline uint8_t getMinute() const
      {
         return minute;
      }

      inline void setMinute( uint8_t p_minute )
      {
         minute = p_minute;
      }

      inline uint8_t getHour() const
      {
         return hour;
      }

      inline void setHour( uint8_t p_hour )
      {
         hour = p_hour;
      }

      inline uint8_t getWeekDay() const
      {
         return weekDay;
      }

      inline void setWeekDay( uint8_t p_weekDay )
      {
         weekDay = p_weekDay;
      }

      inline static WeekTime* getSunRise()
      {
         return (WeekTime*) &sunRise;
      }

      inline static WeekTime* getSunSet()
      {
         return (WeekTime*) &sunSet;
      }

      ////    Attributes    ////

      uint8_t minute;

      uint8_t hour : 5;

      uint8_t weekDay : 3;

      static WeekTime sunRise;

      static WeekTime sunSet;
};

inline uint16_t WeekTime::get()
{
   return *( (uint16_t*) this );
}

inline uint8_t WeekTime::isDay()
{
   return isInRange( sunRise, sunSet );
}

inline uint8_t WeekTime::isSunRise()
{
   return ( ( sunRise.get() & ~WEEK_DAY_MASK ) == ( get() & ~WEEK_DAY_MASK ) );
}

inline uint8_t WeekTime::isSunSet()
{
   return ( ( sunSet.get() & ~WEEK_DAY_MASK ) == ( get() & ~WEEK_DAY_MASK ) );
}

inline void WeekTime::set( uint16_t timestamp )
{
   *( (uint16_t*) this ) = timestamp;
}

#endif
