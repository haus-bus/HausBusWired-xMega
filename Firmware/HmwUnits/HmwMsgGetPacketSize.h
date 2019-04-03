/*
 * HmwMsgGetHwVersion.h
 *
 * Created: 18.11.2017 20:13:05
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGGETPACKETSIZE_H__
#define __HMWMSGGETPACKETSIZE_H__

#include "HmwMessageBase.h"

class HmwMsgGetPacketSize : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline void setupResponse()
      {
         frameData[0] = 0;
         frameData[1] = MAX_PACKET_SIZE;
         frameDataLength = 2;
      }
   protected:
   private:

}; // HmwMsgGetPacketSize

#endif // __HMWMSGGETPACKETSIZE_H__
