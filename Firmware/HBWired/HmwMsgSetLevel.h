/*
 * HmwMsgSetLevel.h
 *
 * Created: 19.11.2017 02:50:10
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGSETLEVEL_H__
#define __HMWMSGSETLEVEL_H__

#include "HmwMessageBase.h"

class HmwMsgSetLevel : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline uint8_t getChannel()
      {
         return frameData[1];
      }

      inline uint8_t const* const getData()
      {
         return &frameData[2];
      }

      inline uint8_t getLength()
      {
         return frameDataLength - 2;
      }
   protected:
   private:

}; // HmwMsgSetLevel

#endif // __HMWMSGSETLEVEL_H__
