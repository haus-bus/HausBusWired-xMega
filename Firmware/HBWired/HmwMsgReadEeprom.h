/*
 * HmwMsgReadEeprom.h
 *
 * Created: 18.11.2017 20:18:28
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGREADEEPROM_H__
#define __HMWMSGREADEEPROM_H__

#include "HmwMessageBase.h"

#include <string.h>

class HmwMsgReadEeprom : public HmwMessageBase
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
         memcpy( frameData, (void*)( MAPPED_EEPROM_START | address ), length );
         // Eeprom::read( address, frameData, length );
         frameDataLength = length;
      }
   protected:
   private:


}; // HmwMsgReadEeprom

#endif // __HMWMSGREADEEPROM_H__
