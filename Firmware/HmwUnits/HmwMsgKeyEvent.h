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

      static const uint8_t PRESS_MASK = 0x03;
      static const uint8_t PRESS_LONG = 0x03;
      static const uint8_t PRESS_SHORT = 0x02;

   protected:
   private:

// functions
   public:
      inline HmwMsgKeyEvent( uint32_t ownAddress, uint32_t dstAddress, uint8_t srcChannel, uint8_t dstChannel, uint8_t keyPressNum, bool longPress )
      {
         frameData[0] = KEY_EVENT;
         frameData[1] = srcChannel;
         frameData[2] = dstChannel;
         frameData[3] = ( longPress ? PRESS_LONG : PRESS_SHORT ) + ( keyPressNum << 2 );
         controlByte = 0xF8;
         senderAddress = ownAddress;
         targetAddress = dstAddress;
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
         return ( frameData[3] & PRESS_MASK ) == PRESS_LONG;
      }

      inline uint8_t getKeyPressNum()
      {
         return ( frameData[3] >> 2 );
      }

   protected:
   private:


}; // HmwMsgKeyEvent

#endif // __HMWMSGKEYEVENT_H__
