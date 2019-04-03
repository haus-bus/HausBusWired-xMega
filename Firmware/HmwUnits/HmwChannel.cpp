/*
 * HmwChannel.cpp
 *
 * Created: 18.11.2017 23:25:12
 * Author: viktor.pankraz
 */


#include "HmwChannel.h"
#include <Time/SystemTime.h>

uint8_t HmwChannel::numChannels = 0;
HmwChannel* HmwChannel::instances[];

HmwChannel::HmwChannel() : type( UNKNOWN ), channelId( numChannels ), nextActionDelay( 1000 )
{
   instances[numChannels++] = this;
}

uint32_t HmwChannel::convertToTime( uint16_t timeValue )
{
   uint8_t factor = ( timeValue >> 14 ); // mask out factor (highest two bits)
   timeValue &= 0x3FFF; // keep time value only

   // factors: 1,60,1000,6000 (last one is not used)
   switch ( factor )
   {
      case 0:      // x0.1s
         return (uint32_t)timeValue * SystemTime::S / 10;
         break;
      case 1:     // x60s
         return (uint32_t)timeValue * SystemTime::MIN;
         break;
      case 2:    // x1000s
         return (uint32_t)timeValue * 1000 * SystemTime::S;
         break;
   }
   return 0;
}

void HmwChannel::set( uint8_t length, uint8_t const* const data )
{
}

uint8_t HmwChannel::get( uint8_t* data )
{
   return 0;
}

void HmwChannel::loop( uint8_t channel )
{
}

void HmwChannel::checkConfig()
{
}