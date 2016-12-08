/*
 * SystemTime.cpp
 *
 *  Created on: 08.12.2016 22:17:53
 *      Author: haus-bus
 */

#include "SystemTime.h"
#include "CriticalSection.h"

SystemTime::time_t SystemTime::now()
{
	time_t time;
	{
		CriticalSection doNotInterrupt;
		time = rtc_get_time();
	}
	return time;
}

void SystemTime::set( time_t value)
{
	CriticalSection doNotInterrupt;
	rtc_set_time( value );
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
    elapsedTime = (timestamp - lastTime);

  return elapsedTime;
}

void SystemTime::waitMs( uint16_t ms )
{
  time_t timestamp = now();
  while ( since( timestamp ) < ms )
    ;
}


