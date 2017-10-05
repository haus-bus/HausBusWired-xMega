/*
 * HmwMessage.cpp
 *
 * Created: 30.09.2017 19:20:46
 * Author: viktor.pankraz
 */


#include "HmwMessage.h"

#include <SwFramework.h>
#include <string.h>

const uint8_t HmwMessage::debugLevel( DEBUG_LEVEL_LOW );

uint32_t HmwMessage::ownAddress( 0 );

// default constructor
HmwMessage::HmwMessage()
{
   memset( this, 0, sizeof( *this ) );
}

void HmwMessage::changeIntoACK()
{
   if ( isInfo() )
   {
      targetAddress = senderAddress;
      senderAddress = ownAddress;
      uint8_t senderNum = controlByte.info.senderNum;
      controlByte = 0x19;
      controlByte.ack.receiverNum = senderNum;
      frameDataLength = 0;
   }
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
      if ( pendingEscape )
      {
         // TODO: Wenn frameEscape gesetzt ist, dann sind das zwei Escapes hintereinander
         // Das ist eigentlich ein Fehler -> Fehlerbehandlung
      }
      pendingEscape = true;
   }
   else
   {
      if ( data == FRAME_STARTBYTE )   // Startzeichen empfangen
      {
         frameStart = true;
         pendingEscape = false;
         frameComplete = false;
         framePointer = 0;
         addressPointer = 0;
         senderAddress = 0;
         targetAddress = 0;
         crc16checksum = 0xFFFF;
         crc16Shift( data, crc16checksum );
      }
      else if ( frameStart )   // Startbyte wurde gefunden und Frame wird nicht ignoriert
      {
         if ( pendingEscape )
         {
            data |= 0x80;
            pendingEscape = false;
         }
         crc16Shift( data, crc16checksum );
         if ( addressPointer < ADDRESS_LENGTH )    // Adressbyte Zieladresse empfangen
         {
            targetAddress <<= 8;
            targetAddress |= data;
            addressPointer++;
         }
         else if ( addressPointer == ADDRESS_LENGTH )    // Controlbyte empfangen
         {
            addressPointer++;
            controlByte = data;
         }
         else if ( controlByte.hasSenderAddress() && addressPointer < ADDRESS_LENGTH_LONG )
         {
            // Adressbyte Sender empfangen wenn CTRL_HAS_SENDER und FRAME_START_LONG
            senderAddress <<= 8;
            senderAddress |= data;
            addressPointer++;
         }
         else if ( addressPointer != 0xFF )    // Datenlänge empfangen
         {
            addressPointer = 0xFF;
            frameDataLength = data;
            if ( frameDataLength > MAX_FRAME_LENGTH )     // Maximale Puffergöße checken.
            {
               frameStart = false;
               DEBUG_M1( FSTR( "E: MsgTooLong" ) );
            }
         }
         else                     // Daten empfangen
         {
            frameData[framePointer] = data;     // Daten in Puffer speichern
            framePointer++;
            if ( framePointer == frameDataLength )     // Daten komplett
            {
               if ( crc16checksum == 0 )      //
               {
                  frameStart = false;
                  frameDataLength -= 2;
                  frameComplete = true;
                  return true;
               }
               else
               {
                  DEBUG_M1( FSTR( "E: CRC" ) );
               }
            }
         }
      }
   }
   return false;
}

bool HmwMessage::getNextByteToSend( uint8_t& data )
{
   uint8_t* dataIdx = &addressPointer;
   bool updateChecksum = true;
   if ( addressPointer == 0 )
   {
      framePointer = 0;
      crc16checksum = 0xFFFF;
      data = FRAME_STARTBYTE;
      crc16Shift( data, crc16checksum );
      DEBUG_M3( FSTR( "T: " ), data, '|' );
      addressPointer++;
      return true;
   }
   else if ( addressPointer <= sizeof( targetAddress ) )
   {
      uint8_t* pTargetAddress = (uint8_t*)&targetAddress;
      data = pTargetAddress[ sizeof( targetAddress ) - addressPointer];
   }
   else if ( addressPointer == ( sizeof( targetAddress ) + 1 ) )
   {
      data = controlByte.get();
   }
   else if ( controlByte.hasSenderAddress() && ( addressPointer <= ( sizeof( targetAddress ) + sizeof( controlByte ) + sizeof( senderAddress ) ) ) )
   {
      uint8_t* pSenderAddress = (uint8_t*)&senderAddress;
      data = pSenderAddress[ sizeof( targetAddress ) + sizeof( controlByte ) + sizeof( senderAddress ) - addressPointer];
   }
   else if ( ( controlByte.hasSenderAddress() && ( addressPointer == ( 1 + sizeof( targetAddress ) + sizeof( controlByte ) + sizeof( senderAddress ) ) ) )
           || ( !controlByte.hasSenderAddress() && ( addressPointer == ( 1 + sizeof( targetAddress ) + sizeof( controlByte ) ) ) ) )
   {
      data = frameDataLength + 2;   // data + 2 bytes checksum
   }
   else
   {
      // sending data now
      dataIdx = &framePointer;
      if ( framePointer < frameDataLength )
      {
         data = frameData[framePointer];
      }
      else if ( framePointer < ( frameDataLength + sizeof( crc16checksum ) ) )
      {
         if ( framePointer == frameDataLength )
         {
            crc16Shift( 0, crc16checksum );
            crc16Shift( 0, crc16checksum );
         }
         uint8_t* pChecksum = (uint8_t*)&crc16checksum;
         data = pChecksum[ 1 + frameDataLength - framePointer];
         updateChecksum = false;
      }
      else
      {
         return false;
      }
   }

   if ( !pendingEscape )
   {
      DEBUG_L2( data, '|' );
      if ( updateChecksum )
      {
         crc16Shift( data, crc16checksum );
      }
      if ( ( data == FRAME_STARTBYTE ) || ( data == FRAME_STARTBYTE_SHORT ) || ( data == ESCAPE_BYTE ) )
      {
         pendingEscape = true;
         data = ESCAPE_BYTE;
      }
      else
      {
         ( *dataIdx )++;
      }
   }
   else
   {
      data &= 0x7F;
      pendingEscape = false;
      ( *dataIdx )++;
   }
   return true;
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