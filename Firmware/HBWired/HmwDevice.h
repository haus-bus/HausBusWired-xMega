/*
 * HmwDevice.h
 *
 * Created: 06.10.2017 22:40:44
 * Author: viktor.pankraz
 */


#ifndef __HMWDEVICE_H__
#define __HMWDEVICE_H__

#include <xEeprom.h>

class HmwDevice
{
// variables
   public:
      struct BasicConfig
      {
         uint8_tx doNotUse;                     // 0x0000
         uint8_tx loggingTime;                  // 0x0001
         uint32_tx centralAddress;              // 0x0002   - 0x0005
         uint32_tx ownAdress;                   // 0x0006   - 0x0009
      };

      static uint8_t deviceType;

      static uint8_t hardwareVersion;

      static uint16_t firmwareVersion;

      static BasicConfig* basicConfig;

      static uint32_t ownAddress;

   protected:
   private:


// functions
   public:
      static inline HmwDevice& instance()
      {
         static HmwDevice hmwDevice;
         return hmwDevice;
      }

   protected:
   private:
      HmwDevice();


}; // HmwDevice

#endif // __HMWDEVICE_H__
