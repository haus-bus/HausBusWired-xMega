/*
 * HmwMsgGetLevel.h
 *
 * Created: 19.11.2017 01:37:26
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGGETLEVEL_H__
#define __HMWMSGGETLEVEL_H__

#include "HmwMessageBase.h"

class HmwMsgGetLevel : public HmwMessageBase
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

      inline uint8_t* getData()
      {
         return &frameData[2];
      }

      inline void setupResponse( uint8_t dataLength )
      {
         frameData[0] = HmwMessageBase::INFO_LEVEL;
         frameDataLength = dataLength + 2;
      }

   protected:

   private:

}; // HmwMsgGetLevel

#endif // __HMWMSGGETLEVEL_H__
