/*
 * HmwMessageBase.h
 *
 * Created: 30.09.2017 19:20:46
 * Author: viktor.pankraz
 */


#ifndef __HMWMESSAGEBASE_H__
#define __HMWMESSAGEBASE_H__

#include <Time/Timestamp.h>
#include <Stream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Release.h>

class HmwMessageBase
{
// variables
   public:

      enum Consts
      {
         FRAME_STARTBYTE = 0xFD,
         FRAME_STARTBYTE_SHORT = 0xFE,
         ESCAPE_BYTE = 0xFC,
         MAX_FRAME_LENGTH = 72,
         ADDRESS_SIZE = 4,
         HEADER_SIZE = 9,

         BROADCAST_ADDR = 0xFFFFFFFF,
         CRC16_POLYNOM = 0x1002
      };

      enum Type
      {
         INFO,
         ACK,
         DISCOVERY
      };

      enum Command
      {
         INVALID = 0,
         RESET = '!',
         ANNOUNCE = 'A',
         READ_CONFIG = 'C',
         GET_EEPROM_MAP = 'E',
         EEPROM_MAP = 'e',
         KEY_EVENT = 'K',
         READ_EEPROM = 'R',
         GET_LEVEL = 'S',
         WRITE_EEPROM = 'W',
         START_DISCOVERY = 'z',
         END_DISCOVERY = 'Z',
         DELETE_TARGET_ADDR = 'c',
         GET_HARDWARE_VERSION = 'h',
         INFO_LEVEL = 'i',
         SET_LOCK = 'l',
         GET_SERIAL = 'n',
         ADD_TARGET_ADDR = 'q',
         SET_ACTOR = 's',
         START_BOOTER = 'u',
         GET_FW_VERSION = 'v',
         WRITE_FLASH = 'w',
         SET_LEVEL = 'x',
         KEY_SIM = 0xCB // 'Ë'
      };

      union ControlByte
      {
         uint8_t controlByte;
         struct Info
         {
            uint8_t infoID : 1; // should be always 0
            uint8_t senderNum : 2;
            uint8_t hasSenderAddr : 1;
            uint8_t isLastPaket : 1;
            uint8_t receiverNum : 2;
            uint8_t sync : 1;
         } info;

         struct Ack
         {
            uint8_t ackID : 3; // should be always 1
            uint8_t hasSenderAddr : 1;
            uint8_t isLastPaket : 1;
            uint8_t receiverNum : 2;
            uint8_t alwaysZero : 1;
         } ack;

         struct Discovery
         {
            uint8_t discoveryID : 3; // should be always 3
            uint8_t addressMask : 5;
         } discovery;

         inline bool isInfo() const
         {
            return info.infoID == 0;
         }

         inline bool isAck() const
         {
            return ack.ackID == 1;
         }

         inline bool isDiscovery() const
         {
            return discovery.discoveryID == 3;
         }

         inline bool hasSenderAddress() const
         {
            return !isDiscovery() && info.hasSenderAddr;
         }

         inline void operator=( uint8_t value )
         {
            controlByte = value;
         }

         inline uint8_t get() const
         {
            return controlByte;
         }
      };

   protected:

      uint32_t targetAddress;
      ControlByte controlByte;
      uint32_t senderAddress;
      uint8_t frameDataLength;
      uint8_t frameData[MAX_FRAME_LENGTH];
      uint8_t valid : 1;
      uint8_t isBigEndian : 1;

   private:

      static const uint8_t debugLevel;


// functions
   public:
      inline HmwMessageBase()
      {
         memset( this, 0, sizeof( *this ) );
      }

      inline void init()
      {
         valid = false;
         isBigEndian = true;
         frameDataLength = 0;
      }

      bool isForMe();

      inline uint8_t getRawByte( uint8_t idx )
      {
         return ( (uint8_t*)this )[idx];
      }

      inline void setRawByte( uint8_t idx, uint8_t data )
      {
         ( (uint8_t*)this )[idx] = data;
      }

      inline Command getCommand()
      {
         return valid ? (Command)frameData[0] : INVALID;
      }

      inline bool isCommand( Command cmd )
      {
         return valid && ( cmd == frameData[0] );
      }

      inline bool isACK()
      {
         return controlByte.isAck();
      }

      inline bool isDiscovery()
      {
         return controlByte.isDiscovery();
      }

      inline bool isInfo()
      {
         return controlByte.isInfo();
      }

      inline bool isSync()
      {
         // attention: this is only valid for messages of type Info
         return controlByte.info.sync;
      }

      inline bool isBroadcast()
      {
         return targetAddress == BROADCAST_ADDR;
      }

      inline bool isValid()
      {
         return valid;
      }

      inline void setValid( bool _valid )
      {
         valid = _valid;
      }

      inline void convertToHmwSerialString( uint32_t address, uint8_t* buffer )
      {
         buffer[0] = 'H';
         buffer[1] = 'B';
         buffer[2] = 'W';

         // string has max 7 digits
         uint8_t* pEnd = &buffer[9];
         while ( pEnd != &buffer[2] )
         {
            *pEnd-- = '0' + ( address % 10 );
            if ( address )
            {
               address /= 10;
            }
         }
      }

      inline void convertToLittleEndian()
      {
         if ( isBigEndian )
         {
            targetAddress = ::changeEndianness( targetAddress );
            senderAddress = ::changeEndianness( senderAddress );
            isBigEndian = false;
         }
      }

      inline void convertToBigEndian()
      {
         if ( !isBigEndian )
         {
            targetAddress = ::changeEndianness( targetAddress );
            senderAddress = ::changeEndianness( senderAddress );
            isBigEndian = true;
         }
      }

      inline void convertToResponse( uint32_t& ownAddress, bool isAck )
      {
         targetAddress = senderAddress;
         senderAddress = ownAddress;
         uint8_t tmp = getSenderNum();

         if ( isAck )
         {
            controlByte = 0x19;
            frameDataLength = 0;
         }
         else
         {
            controlByte = 0x98;
         }
         controlByte.info.receiverNum = tmp;
      }

      inline bool hasSenderAddress() const
      {
         return controlByte.hasSenderAddress();
      }

      inline const uint32_t& getSenderAddress() const
      {
         return senderAddress;
      }

      inline const uint32_t& getTargetAddress() const
      {
         return targetAddress;
      }

      inline uint8_t getSenderNum() const
      {
         // attention: this is only valid for messages of type Info
         return controlByte.info.senderNum;
      }

      inline void setSenderNum( uint8_t value )
      {
         // attention: this is only valid for messages of type Info
         controlByte.info.senderNum = value;
      }

      inline void setReceiverNum( uint8_t value )
      {
         controlByte.info.receiverNum = value;
      }

      inline uint8_t getFrameDataLength() const
      {
         return frameDataLength;
      }

      inline void setFrameDataLength( uint8_t len )
      {
         frameDataLength = len;
      }

      inline void setSyncBit()
      {
         controlByte.info.sync = true;
      }

      static void crc16Shift( uint8_t newByte, uint16_t& crc );

   protected:
   private:

}; // HmwMessage

#endif // __HMWMESSAGE_H__
