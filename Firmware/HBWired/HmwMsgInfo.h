/*
 * HmwMsgInfo.h
 *
 * Created: 20.11.2017 02:45:27
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGINFO_H__
#define __HMWMSGINFO_H__

#include "HmwMessageBase.h"
#include <string.h>

class HmwMsgInfo : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline HmwMsgInfo( uint32_t ownAddress, uint32_t dstAddress, uint8_t srcChannel, uint8_t const* const data, uint8_t length )
      {
         frameData[0] = INFO_LEVEL;
         frameData[1] = srcChannel;
         memcpy( &frameData[2], data, length );

         controlByte = 0xF8;
         senderAddress = ownAddress;
         targetAddress = dstAddress;
         frameDataLength = 0x02 + length;

      }
   protected:
   private:

}; // HmwMsgInfo

#endif // __HMWMSGINFO_H__
