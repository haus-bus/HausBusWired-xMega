/*
 * SystemTime.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SystemTime_H
#define SystemTime_H

#include <stdint.h>

class SystemTime
{
   public:

      enum ClockSources
      {
         RTCSRC_ULP_1024,
         RTCSRC_TOSC_1024,
         RTCSRC_RCOSC_1024,
         RTCSRC_TOSC_32768,
         RTCSRC_RCOSC_32768,
         RTCSRC_EXTCLK,
      };

#ifndef SYSTEM_TIME_TYPE
#define SYSTEM_TIME_TYPE uint32_t
#endif

      typedef SYSTEM_TIME_TYPE time_t;

      static const uint8_t MS = 1;
      static const uint16_t S = 1024 * MS;
      static const uint16_t MIN = 60 * S;
      static const uint32_t H = 60 * MIN;
      static const uint32_t D = 24 * H;

      static int8_t ticksPerSecondAdjustment;

      ////    Operations    ////

      static void init( ClockSources cs = RTCSRC_RCOSC_1024, uint16_t frequency = 1024 );

      static time_t now();

      static void set( time_t value );

      static time_t since( const time_t& lastTime );

      static void waitMs( uint16_t ms );

      ////    Attributes    ////

   private:

      static uint16_t ticks;
};

#endif
