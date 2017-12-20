/*
 * HmwMsgAnnounce.h
 *
 * Created: 13.11.2017 10:26:37
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGANNOUNCE_H__
#define __HMWMSGANNOUNCE_H__

#include "HmwMessageBase.h"


class HmwMsgAnnounce : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline HmwMsgAnnounce( uint8_t channel, uint32_t ownAddress, uint8_t deviceType, uint8_t hardwareVersion, uint16_t firmwareVersion )
      {
         senderAddress = ownAddress;
         targetAddress = 0xFFFFFFFF;
         controlByte = 0xF8;
         frameData[0] = ANNOUNCE;
         frameData[1] = channel;
         frameData[2] = deviceType;
         frameData[3] = hardwareVersion;
         frameData[4] = firmwareVersion / 0x100;
         frameData[5] = firmwareVersion & 0xFF;
         convertToHmwSerialString( ownAddress, &frameData[6] );
         frameDataLength = 16;
      }

   protected:
   private:


}; // HmwMsgAnnounce

#endif // __HMWMSGANNOUNCE_H__
