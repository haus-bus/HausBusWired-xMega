/*
 * HmwMsgGetFwVersion.h
 *
 * Created: 18.11.2017 20:08:35
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGGETFWVERSION_H__
#define __HMWMSGGETFWVERSION_H__

#include "HmwMessageBase.h"


class HmwMsgGetFwVersion : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline void setupResponse( uint16_t fwVersion )
      {
         frameData[0] = fwVersion >> 8;
         frameData[1] = fwVersion & 0xFF;
         frameDataLength = 2;
      }
   protected:
   private:


}; // HmwMsgGetFwVersion

#endif // __HMWMSGGETFWVERSION_H__
