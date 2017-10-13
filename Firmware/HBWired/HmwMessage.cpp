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

bool HmwMessage::generateResponse()
{
   if ( !isInfo() )
   {
      return false;
   }

   bool isValid = true;
   uint8_t msgSenderNum = controlByte.info.senderNum;

   // if sync bit is set, reset the static senderNum
   if ( controlByte.info.sync )
   {
      senderNum = 0;
      receiverNum = controlByte.info.senderNum;
   }
   if ( isCommand( READ_CONFIG ) )
   {
      DEBUG_M1( FSTR( "C: read config" ) );
      changeIntoACK();
   }
   else if ( isCommand( GET_FW_VERSION ) )
   {
      DEBUG_M1( FSTR( "C: get FW version" ) );
      frameData[0] = HmwDevice::firmwareVersion >> 8;
      frameData[1] = HmwDevice::firmwareVersion& 0xFF;
      frameDataLength = 2;
      controlByte = 0x18;
   }
   else if ( isCommand( GET_HARDWARE_VERSION ) )
   {
      DEBUG_M1( FSTR( "T: HWVer,Typ" ) );
      frameData[0] = HmwDevice::deviceType;
      frameData[1] = HmwDevice::hardwareVersion;
      frameDataLength = 2;
      controlByte = 0x18;
   }
   else if ( isCommand( READ_EEPROM ) )
   {
      DEBUG_M1( FSTR( "C: Read EEPROM" ) );
      if ( frameDataLength == 4 )    // Length of incoming data must be 4
      {
         uint16_t address = ( frameData[1] << 8 ) | frameData[2];
         uint8_t length = frameData[3];
         memcpy( frameData, (void*)( MAPPED_EEPROM_START | address ), length );
         frameDataLength = length;
         controlByte = 0x18;
      }
      else
      {
         DEBUG_M2( FSTR( "E: wrong data length :" ), frameDataLength );
         isValid = false;
      }

   }
   else if ( isCommand( WRITE_FLASH ) )
   {
      DEBUG_M1( FSTR( "C: Write Flash" ) );
      if ( frameDataLength > 6 )
      {
         static uint8_t buffer[ APP_SECTION_PAGE_SIZE ];
         Flash::address_t address = ( frameData[3] << 8 ) | frameData[4];
         uint8_t length = frameData[5];
         memcpy( &buffer[address & ( Flash::getPageSize() - 1 )], &frameData[6], length );

         if ( ( address + length ) % Flash::getPageSize() )
         {
            // read more bytes
            changeIntoACK();
         }
         else if ( Flash::write( address & ~( Flash::getPageSize() - 1 ), buffer, Flash::getPageSize() ) == Flash::getPageSize() )
         {
            changeIntoACK();
         }
         else
         {
            DEBUG_M2( FSTR( "E: Flash::write failed:" ), frameDataLength );
            isValid = false;
         }
      }
      else
      {
         DEBUG_M2( FSTR( "E: wrong data length :" ), frameDataLength );
         isValid = false;
      }

   }
   else
   {
      return false;
   }

   if ( isValid )
   {
      targetAddress = senderAddress;
      senderAddress = HmwDevice::ownAddress;
      controlByte.info.receiverNum = msgSenderNum;
      if ( isInfo() )
      {
         controlByte.info.senderNum = senderNum;
      }
   }
   return isValid;
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

void HmwMessage::setupAnnounce( uint8_t channel )
{
   senderAddress = HmwDevice::ownAddress;
   targetAddress = 0xFFFFFFFF;
   controlByte = 0xF8;
   frameData[0] = ANNOUNCE;
   frameData[1] = channel;
   frameData[2] = HmwDevice::deviceType;
   frameData[3] = HmwDevice::hardwareVersion;
   frameData[4] = HmwDevice::firmwareVersion / 0x100;
   frameData[5] = HmwDevice::firmwareVersion& 0xFF;
   convertToHmwSerialString( HmwDevice::ownAddress, &frameData[6] );
   frameDataLength = 16;
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