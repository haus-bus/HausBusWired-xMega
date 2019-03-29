/*
 * HBWX32A4DeviceFactory.cpp
 *
 * Created: 04.12.2018 13:58:59
 * Author: viktor.pankraz
 */


#include "HBWDeviceFactory.h"
#include "HBWLcDim8BaseHw.h"

#include <Peripherals/Flash.h>

HmwDeviceHw* HBWDeviceFactory::createDevice( Release::FirmwareId firmwareId )
{
   HmwDeviceHw::BasicConfig& config = *reinterpret_cast<HmwDeviceHw::BasicConfig*>( MAPPED_EEPROM_START );

   // allow also 0xFF that is REV_0 but with an old booter, that did not set correct revision
   if ( config.hwVersion == 0xFF )
   {
      // update the cell so that HW_REV is reported correctly
      config.hwVersion = Release::REV_0;
   }

   if ( firmwareId == Release::HBW_LC_DIM8_1W_DR )
   {
      if ( config.hwVersion == Release::REV_0 )
      {
         return new HBWLcDim8BaseHw();
      }
   }

   // if we get here, the FW does not support this device -> erase it and return to booter FW
   Flash::eraseApplication();

   while ( 1 )
   {
   }
}



