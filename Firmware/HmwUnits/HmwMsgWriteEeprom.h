/*
 * HmwMsgWriteEeprom.h
 *
 * Created: 19.11.2017 02:04:37
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGWRITEEEPROM_H__
#define __HMWMSGWRITEEEPROM_H__

#include "HmwMessageBase.h"

#include <string.h>

class HmwMsgWriteEeprom : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline uint16_t getOffset()
      {
         return ( frameData[1] << 8 ) | frameData[2];
      }

      inline uint8_t getLength()
      {
         return frameData[3];
      }

      inline uint8_t* getData()
      {
         return &frameData[4];
      }

   protected:

   private:


}; // HmwMsgWriteEeprom

#endif // __HMWMSGWRITEEEPROM_H__
