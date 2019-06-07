/*
 * HmwMsgStartupReason.h
 *
 * Created: 06.06.2019 02:50:10
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGSTARTUPREASON_H__
#define __HMWMSGSTARTUPREASON_H__

#include "HmwMessageBase.h"

class HmwMsgStartupReason : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline HmwMsgStartupReason( uint32_t ownAddress, uint8_t reason )
      {
         senderAddress = ownAddress;
         targetAddress = 0xFFFFFFFF;
         controlByte = 0xF8;
         frameData[0] = STARTUP_REASON;
         frameData[1] = reason;
         frameDataLength = 2;
      }
   protected:
   private:

}; // HmwMsgStartupReason

#endif // __HMWMSGSTARTUPREASON_H__
