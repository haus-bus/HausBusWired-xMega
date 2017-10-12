/*
 * HmwMessage.h
 *
 * Created: 30.09.2017 19:20:46
 * Author: viktor.pankraz
 */


#ifndef __HMWMESSAGE_H__
#define __HMWMESSAGE_H__

#include <Time/Timestamp.h>
#include <stdio.h>

class HmwMessage
{
// variables
   public:

      enum Consts
      {
         FRAME_STARTBYTE = 0xFD,
         FRAME_STARTBYTE_SHORT = 0xFE,
         ESCAPE_BYTE = 0xFC,

         ADDRESS_LENGTH = 4,
         ADDRESS_LENGTH_LONG = 9,
         MAX_FRAME_LENGTH = 72,

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
         UNKNOWN_E = 'E',
         RESPONSE_e = 'e',
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
         UNKNOWN_n = 'n',
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

         inline bool isInfo()
         {
            return info.infoID == 0;
         }

         inline bool isAck()
         {
            return ack.ackID == 1;
         }

         inline bool isDiscovery()
         {
            return discovery.discoveryID == 3;
         }

         inline bool hasSenderAddress()
         {
            return !isDiscovery() && info.hasSenderAddr;
         }

         inline void operator=( uint8_t value )
         {
            controlByte = value;
         }

         inline uint8_t get()
         {
            return controlByte;
         }
      };

   protected:

      struct Status
      {
         uint8_t dataIdx;
         uint8_t pendingEscape : 1;
         uint8_t sending       : 1;
         uint8_t receiving     : 1;
         uint8_t frameValid    : 1;
      } status;

      uint32_t targetAddress;
      ControlByte controlByte;
      uint32_t senderAddress;
      uint8_t frameDataLength;
      uint8_t frameData[MAX_FRAME_LENGTH];
      uint16_t crc16checksum;

   private:

      static const uint8_t debugLevel;

      static uint8_t senderNum;

      static uint8_t receiverNum;

// functions
   public:
      HmwMessage();

      bool generateResponse();

      bool nextByteReceived( uint8_t data );

      bool getNextByteToSend( uint8_t& data );

      void changeIntoACK();

      bool isForMe();

      inline Command getCommand()
      {
         return status.frameValid ? (Command)frameData[0] : INVALID;
      }

      inline bool isCommand( Command cmd )
      {
         return status.frameValid && ( cmd == frameData[0] );
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

      inline bool isBroadcast()
      {
         return targetAddress == 0xFFFFFFFF;
      }

      inline void prepareToSend()
      {
         targetAddress = changeEndianness( targetAddress );
         senderAddress = changeEndianness( senderAddress );
         frameDataLength += 2;
         status.dataIdx = 0;
         status.sending = false;
      }

      void setupAnnounce( uint8_t channel );

      inline void convertToHmwSerialString( uint32_t address, uint8_t* buffer )
      {
         buffer[0] = 'H';
         buffer[1] = 'B';
         buffer[2] = 'W';

         for ( uint8_t i = 9; i > 2; i-- )
         {
            if ( address )
            {
               buffer[i] = '0' + ( address % 10 );
            }
            else
            {
               buffer[i] = '0';
            }
            address /= 10;
         }
      }

   protected:
   private:

      void crc16Shift( uint8_t newByte, uint16_t& crc );

}; // HmwMessage

#endif // __HMWMESSAGE_H__
