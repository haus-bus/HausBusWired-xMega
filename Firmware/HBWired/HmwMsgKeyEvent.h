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
