/*
 * HBWDeviceFactory.h
 *
 * Created: 09.01.2018 21:04:36
 * Author: viktor.pankraz
 */


#ifndef __HMWDEVICEFACTORY_H__
#define __HMWDEVICEFACTORY_H__

#include <HmwUnits/HmwDevice.h>

class HBWDeviceFactory
{
// variables
   public:

      static HmwDeviceHw* createDevice( Release::FirmwareId firmwareId );

}; // HBWDeviceFactory

#endif // __HMWDEVICEFACTORY_H__
