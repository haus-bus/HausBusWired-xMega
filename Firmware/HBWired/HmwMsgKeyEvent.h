/*
 * HmwMsgKeyEvent.h
 *
 * Created: 19.11.2017 00:56:45
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGKEYEVENT_H__
#define __HMWMSGKEYEVENT_H__

#include "HmwMessageBase.h"

class HmwMsgKeyEvent : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline HmwMsgKeyEvent( uint32_t ownAddress, uint32_t dstAddress, uint8_t srcChannel, uint8_t dstChannel, uint8_t keyPressNum, bool longPress )
      {
         frameData[0] = KEY_EVENT;
         frameData[1] = srcChannel;
         frameData[2] = dstChannel;
         frameData[3] = ( longPress ? 3 : 2 ) + ( keyPressNum << 2 );

         senderAddress = ownAddress;
         targetAddress = dstAddress;
         controlByte = 0x18;
         frameDataLength = 0x04;
      }

      inline uint8_t getSourceChannel()
      {
         return frameData[1];
      }

      inline uint8_t getDestinationChannel()
      {
         return frameData[2];
      }

      inline bool isLongPress()
      {
         return frameData[3];
      }
   protected:
   private:


}; // HmwMsgKeyEvent

#endif // __HMWMSGKEYEVENT_H__
