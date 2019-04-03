/*
 * HmwMsgReadEeprom.h
 *
 * Created: 18.11.2017 20:18:28
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGREADFLASH_H__
#define __HMWMSGREADFLASH_H__

#include "HmwMessageBase.h"

#include <string.h>

class HmwMsgReadFlash : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline void setupResponse()
      {
         uint16_t address = ( frameData[1] << 8 ) | frameData[2];
         uint8_t length = frameData[3];
         frameDataLength = Flash::read( address, &frameData[0], length );
      }
   protected:
   private:


}; // HmwMsgReadFlash

#endif // __HMWMSGREADFLASH_H__
