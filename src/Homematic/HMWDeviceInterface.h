/* 
* HMWDeviceBase.h
*
* Created: 11.12.2016 14:29:11
* Author: haus-bus
*/


#ifndef __HMWDEVICEBASE_H__
#define __HMWDEVICEBASE_H__


class HMWDeviceInterface
{
public:
	virtual void setLevel( uint8_t, unsigned int ) = 0; // channel, level
	virtual unsigned int getLevel( uint8_t ) = 0;       // channel, returns level
	virtual void readConfig() = 0;						// read config from EEPROM
}; //HMWDeviceBase

#endif //__HMWDEVICEBASE_H__
