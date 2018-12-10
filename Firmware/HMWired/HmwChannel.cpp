/*
 * HmwChannel.cpp
 *
 * Created: 18.11.2017 23:25:12
 * Author: viktor.pankraz
 */


#include "HmwChannel.h"

uint8_t HmwChannel::numChannels = 0;
HmwChannel* HmwChannel::instances[];

HmwChannel::HmwChannel() : type( UNKNOWN ), channelId( numChannels )
{
   instances[numChannels++] = this;
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