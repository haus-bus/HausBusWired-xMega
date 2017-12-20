/*
 * HmwMsgWriteFlash.h
 *
 * Created: 18.11.2017 20:37:11
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGWRITEFLASH_H__
#define __HMWMSGWRITEFLASH_H__

#include "HmwMessageBase.h"

#include <Peripherals/Flash.h>

class HmwMsgWriteFlash : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline Flash::address_t getAddress()
      {
         return ( frameData[3] << 8 ) | frameData[4];
      }

      inline uint8_t getLength()
      {
         return frameData[5];
      }

      inline uint8_t* getData()
      {
         return &frameData[6];
      }
   protected:
   private:


}; // HmwMsgWriteFlash

#endif // __HMWMSGWRITEFLASH_H__
