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

const uint8_t HmwMessageBase::debugLevel( DEBUG_LEVEL_OFF );

uint8_t HmwMessageBase::messagesInUse( 0 );

bool HmwMessageBase::isForMe() const
{
   return valid && ( ( targetAddress == HmwDevice::ownAddress ) || isBroadcast() );
}

bool HmwMessageBase::isOnlyForMe() const
{
   return ( targetAddress == HmwDevice::ownAddress );
}

bool HmwMessageBase::isFromMe() const
{
   return ( senderAddress == HmwDevice::ownAddress );
}

void HmwMessageBase::operator delete( void* obj, size_t size )
{
   CriticalSection cs;
   delete (uint8_t*) obj;
   if ( messagesInUse )
   {
      messagesInUse--;
   }
   DEBUG_M4( FSTR( "del " ), (uintptr_t ) obj, FSTR( " use " ), messagesInUse );
}

HmwMessageBase* HmwMessageBase::copy() const
{
   CriticalSection doNotInterrupt;
   uint8_t completeLength = getObjectSize();
   HmwMessageBase* newMsg = (HmwMessageBase*) new uint8_t[( completeLength + 7 ) & 0xFFF8];
   if ( newMsg )
   {
      memcpy( (uint8_t*) newMsg, this, completeLength );
      messagesInUse++;
      DEBUG_M4( FSTR( "new " ), (uintptr_t ) newMsg, FSTR( " use " ), messagesInUse );
   }
   return newMsg;
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