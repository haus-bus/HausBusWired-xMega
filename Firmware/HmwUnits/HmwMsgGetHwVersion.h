/*
 * HmwMsgGetHwVersion.h
 *
 * Created: 18.11.2017 20:13:05
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGGETHWVERSION_H__
#define __HMWMSGGETHWVERSION_H__

#include "HmwMessageBase.h"

class HmwMsgGetHwVersion : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline void setupResponse( uint8_t deviceType, uint8_t hwVersion )
      {
         frameData[0] = deviceType;
         frameData[1] = hwVersion;
         frameDataLength = 2;
      }
   protected:
   private:

}; // HmwMsgGetHwVersion

#endif // __HMWMSGGETHWVERSION_H__
