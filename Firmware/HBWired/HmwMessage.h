/*
 * HmwMessage.h
 *
 * Created: 30.09.2017 19:20:46
 * Author: viktor.pankraz
 */


#ifndef __HMWMESSAGE_H__
#define __HMWMESSAGE_H__

#include <Time/Timestamp.h>

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
         MAX_FRAME_LENGTH = 64,

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
         SET_LEVEL = 'x',
         KEY_SIM = 0xCB // 'Ë'
      };

      union ControlByte
      {
         uint8_t controlByte;
         struct Info
         {
            uint8_t sync : 1;
            uint8_t receiverNum : 2;
            uint8_t isLastPaket : 1;
            uint8_t hasSenderAddr : 1;
            uint8_t senderNum : 2;
            uint8_t infoID : 1; // should be always 0
         } info;

         struct Ack
         {
            uint8_t alwaysZero : 1;
            uint8_t receiverNum : 2;
            uint8_t isLastPaket : 1;
            uint8_t hasSenderAddr : 1;
            uint8_t ackID : 3; // should be always 1
         } ack;

         struct Discovery
         {
            uint8_t addressMask : 5;
            uint8_t discoveryID : 3; // should be always 3
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

      uint32_t senderAddress;
      uint32_t targetAddress;
      uint16_t crc16checksum;
      uint8_t framePointer;
      uint8_t addressPointer;
      uint8_t frameDataLength;
      uint8_t frameData[MAX_FRAME_LENGTH];
      ControlByte controlByte;
      bool pendingEscape;
      bool frameStart;
      bool frameComplete;

   private:

      static const uint8_t debugLevel;

// functions
   public:
      HmwMessage();

      bool nextByteReceived( uint8_t data );

      bool getNextByteToSend( uint8_t& data );

      void changeIntoACK();

      inline bool isForMe( uint32_t address )
      {
         return frameComplete && ( ( targetAddress == address ) || isBroadcast() );
      }

      inline Command getCommand()
      {
         return frameComplete ? (Command)frameData[0] : INVALID;
      }

      inline bool isCommand( Command cmd )
      {
         return frameComplete && ( cmd == frameData[0] );
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
         addressPointer = 0;
         framePointer = 0;
      }

   protected:
   private:
      HmwMessage( const HmwMessage& c );
      HmwMessage& operator=( const HmwMessage& c );

      void crc16Shift( uint8_t newByte, uint16_t& crc );

}; // HmwMessage

#endif // __HMWMESSAGE_H__
