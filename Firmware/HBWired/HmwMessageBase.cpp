/*
 * HmwMessageBase.cpp
 *
 * Created: 30.09.2017 19:20:46
 * Author: viktor.pankraz
 */


#include "HmwMessageBase.h"
#include "HmwDevice.h"

#include <Peripherals/Flash.h>
#include <SwFramework.h>
#include <string.h>

const uint8_t HmwMessageBase::debugLevel( DEBUG_LEVEL_LOW );

// default constructor
HmwMessageBase::HmwMessageBase()
{
   memset( this, 0, sizeof( *this ) );
}

void HmwMessageBase::changeIntoACK()
{
   if ( isInfo() )
   {
      controlByte = 0x19;
      frameDataLength = 0;
   }
}

bool HmwMessageBase::isForMe()
{
   return valid && ( ( targetAddress == HmwDevice::ownAddress ) || isBroadcast() );
}


// calculate crc16 checksum for each byte
void HmwMessageBase::crc16Shift( uint8_t newByte, uint16_t& crc )
{
   uint8_t stat;

   for ( uint8_t i = 0; i < 8; i++ )
   {
      if ( crc & 0x8000 )
      {
         stat = 1;
      }
      else
      {
         stat = 0;
      }
      crc <<= 1;
      if ( newByte & 0x80 )
      {
         crc |= 1;
      }
      if ( stat )
      {
         crc ^= CRC16_POLYNOM;
      }
      newByte = newByte << 1;
   }
}  // crc16Shift