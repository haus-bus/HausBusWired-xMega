/*
 * SystemTime.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SystemTime_H
#define SystemTime_H

extern "C"
{
    #include <asf.h>
}

class SystemTime
{
public:


#ifndef SYSTEM_TIME_TYPE
#define SYSTEM_TIME_TYPE uint32_t
#endif

  typedef SYSTEM_TIME_TYPE time_t;

  static const uint8_t MS = 1;
  static const uint16_t S = 1024 * MS;
  static const uint16_t MIN = 60 * S;
  static const uint32_t H = 60 * MIN;
  static const uint32_t D = 24 * H;

  ////    Operations    ////

  static inline void init()
  {
    rtc_init();
  }

  static inline time_t now()
  {
    return rtc_get_time();
  }

  static inline void set( time_t value )
  {
    rtc_set_time( value );
  }

  static time_t since( const time_t& lastTime );

  static void waitMs( uint16_t ms );

  ////    Attributes    ////

private:

};

#endif
