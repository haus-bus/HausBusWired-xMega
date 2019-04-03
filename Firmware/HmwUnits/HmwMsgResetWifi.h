/*
 * HmwMsgGetFwVersion.h
 *
 * Created: 18.11.2017 20:08:35
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGRESETWIFI_H__
#define __HMWMSGRESETWIFI_H__

#include "HmwMessageBase.h"


class HmwMsgResetWifi : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline HmwMsgResetWifi( uint32_t ownAddress, uint32_t dstAddress )
      {
         senderAddress = ownAddress;
         targetAddress = dstAddress;
         controlByte = 0xF8;
         frameData[0] = RESET_WIFI;
         frameDataLength = 1;
      }
   protected:
   private:


}; // HmwMsgGetFwVersion

#endif // __HMWMSGGETFWVERSION_H__
