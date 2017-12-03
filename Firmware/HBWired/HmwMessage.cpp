/*
 * HmwMessage.cpp
 *
 * Created: 30.09.2017 19:20:46
 * Author: viktor.pankraz
 */


#include "HmwMessage.h"
#include "HmwDevice.h"

#include <Peripherals/Flash.h>
#include <SwFramework.h>
#include <string.h>

const uint8_t HmwMessage::debugLevel( DEBUG_LEVEL_LOW );

uint8_t HmwMessage::senderNum( 0 );
uint8_t HmwMessage::receiverNum( 0 );

// default constructor
HmwMessage::HmwMessage()
{
   memset( this, 0, sizeof( *this ) );
}

void HmwMessage::changeIntoACK()
{
   if ( isInfo() )
   {
      controlByte = 0x19;
      frameDataLength = 0;
   }
}

bool HmwMessage::isForMe()
{
   return status.frameValid && ( ( targetAddress == HmwDevice::ownAddress ) || isBroadcast() );
}

bool HmwMessage::nextByteReceived( uint8_t data )
{
   // Debug
   if ( data == FRAME_STARTBYTE )
   {
      DEBUG_M1( FSTR( "R: " ) );
   }
   else
   {
      DEBUG_L1( FSTR( ":" ) );
   }
   DEBUG_L1( data );

   if ( data == ESCAPE_BYTE )
   {
      if ( status.pendingEscape )
      {
         // TODO: Wenn frameEscape gesetzt ist, dann sind das zwei Escapes hintereinander
         // Das ist eigentlich ein Fehler -> Fehlerbehandlung
      }
      status.pendingEscape = true;
   }
   else
   {
      if ( data == FRAME_STARTBYTE )   // Startzeichen empfangen
      {
         status.receiving = true;
         status.pendingEscape = false;
         status.frameValid = false;
         status.dataIdx = 0;
         frameDataLength = 0;
         crc16checksum = 0xFFFF;
         crc16Shift( data, crc16checksum );
      }
      else if ( status.receiving )   // Startbyte wurde gefunden und Frame wird nicht ignoriert
      {
         if ( status.pendingEscape )
         {
            data |= 0x80;
            status.pendingEscape = false;
         }
         crc16Shift( data, crc16checksum );
         uint8_t* pData = (uint8_t*)&targetAddress;
         pData[status.dataIdx] = data;

         if ( status.dataIdx == sizeof( targetAddress ) )
         {
            // controlByte was read
            if ( !controlByte.hasSenderAddress() )
            {
               status.dataIdx += sizeof( senderAddress );
            }
         }
         else if ( status.dataIdx == ( sizeof( targetAddress ) + sizeof( controlByte ) + sizeof( senderAddress ) ) )
         {
            // frameDataLength was read
            if ( frameDataLength > MAX_FRAME_LENGTH )       // Maximale Puffergöße checken
            {
               status.receiving = false;
               DEBUG_M1( FSTR( "E: MsgTooLong" ) );
            }
         }
         else if ( status.dataIdx == ( sizeof( targetAddress ) + sizeof( controlByte ) + sizeof( senderAddress ) + frameDataLength ) )
         {
            // Daten komplett
            if ( crc16checksum == 0 )      //
            {
               targetAddress = changeEndianness( targetAddress );
               senderAddress = changeEndianness( senderAddress );
               frameDataLength -= 2;
               status.receiving = false;
               status.frameValid = true;
               return true;
            }
            else
            {
               DEBUG_M1( FSTR( "E: CRC" ) );
            }
         }
         status.dataIdx++;
      }
   }
   return false;
}

bool HmwMessage::getNextByteToSend( uint8_t& data )
{
   bool updateChecksum = true;
   if ( ( status.dataIdx == 0 ) && !status.sending )
   {
      status.sending = true;
      crc16checksum = 0xFFFF;
      data = FRAME_STARTBYTE;
      crc16Shift( data, crc16checksum );
      DEBUG_M3( FSTR( "T: " ), data, '|' );
      return true;
   }
   else if ( status.sending )
   {
      uint8_t* pData = (uint8_t*)&targetAddress;
      data = pData[status.dataIdx];

      if ( status.dataIdx == sizeof( targetAddress ) )
      {
         // controlByte was sent
         if ( !controlByte.hasSenderAddress() )
         {
            status.dataIdx += sizeof( senderAddress );
         }
      }
      else if ( status.dataIdx == ( sizeof( targetAddress ) + sizeof( controlByte ) + sizeof( senderAddress ) + frameDataLength - sizeof( crc16checksum ) ) )
      {
         // last data byte was sent, calculate crc
         crc16Shift( 0, crc16checksum );
         crc16Shift( 0, crc16checksum );
         pData[status.dataIdx + 1] = HBYTE( crc16checksum );
         pData[status.dataIdx + 2] = LBYTE( crc16checksum );
      }
      else if ( status.dataIdx > ( sizeof( targetAddress ) + sizeof( controlByte ) + sizeof( senderAddress ) + frameDataLength ) )
      {
         // sending complete, no data available
         return false;
      }

      if ( !status.pendingEscape )
      {
         DEBUG_L2( data, '|' );
         if ( updateChecksum )
         {
            crc16Shift( data, crc16checksum );
         }
         if ( ( data == FRAME_STARTBYTE ) || ( data == FRAME_STARTBYTE_SHORT ) || ( data == ESCAPE_BYTE ) )
         {
            status.pendingEscape = true;
            data = ESCAPE_BYTE;
         }
         else
         {
            status.dataIdx++;
         }
      }
      else
      {
         data &= 0x7F;
         status.pendingEscape = false;
         status.dataIdx++;
      }
      return true;
   }

   return false;
}

// calculate crc16 checksum for each byte
void HmwMessage::crc16Shift( uint8_t newByte, uint16_t& crc )
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