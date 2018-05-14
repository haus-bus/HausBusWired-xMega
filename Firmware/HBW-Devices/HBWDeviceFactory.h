/*
 * HBWDeviceFactory.h
 *
 * Created: 09.01.2018 21:04:36
 * Author: viktor.pankraz
 */


#ifndef __HMWDEVICEFACTORY_H__
#define __HMWDEVICEFACTORY_H__


#include "HBWMultiKeySD6v0Hw.h"
#include "HBWMultiKeySD6v1Hw.h"
#include "HBWIo12OneWireUpHw.h"
#include "HBWLC4IN4DRHw.h"

class HBWDeviceFactory
{
// variables
   public:

      static inline HmwDeviceHw* createDevice( Release::FirmwareId firmwareId )
      {
         HmwDeviceHw::BasicConfig& config = *reinterpret_cast<HmwDeviceHw::BasicConfig*>( MAPPED_EEPROM_START );

         // allow also 0xFF that is REV_0 but with an old booter, that did not set correct revision
         if ( config.hwVersion == 0xFF )
         {
            // update the cell so that HW_REV is reported correctly
            config.hwVersion = Release::REV_0;
         }

         if ( firmwareId == Release::HBW_MultiKeySD6 )
         {
            if ( config.hwVersion == Release::REV_0 )
            {
               return new HBWMultiKeySD6v0Hw();
            }
            if ( config.hwVersion == Release::REV_1 )
            {
               return new HBWMultiKeySD6v1Hw();
            }
         }

         if ( firmwareId == Release::HBW_IO12_1W_UP )
         {
            if ( ( config.hwVersion == Release::REV_0 ) || ( config.hwVersion == Release::REV_1 ) )
            {
               return new HBWIo12OneWireUpHw();
            }
         }

         if ( firmwareId == Release::HBW_LC4_IN4_DR )
         {
            if ( config.hwVersion == Release::REV_0 )
            {
               return new HBWLC4IN4DRHw();
            }
         }

         while ( 1 )
         {
         }
      }

}; // HBWDeviceFactory

#endif // __HMWDEVICEFACTORY_H__
