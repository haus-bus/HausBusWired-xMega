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


}; // HmwMsgWriteFlash

#endif // __HMWMSGWRITEFLASH_H__
