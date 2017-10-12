/*
 * HmwDevice.cpp
 *
 * Created: 06.10.2017 22:40:44
 * Author: viktor.pankraz
 */


#include "HmwDevice.h"

uint8_t HmwDevice::deviceType( 0 );

uint8_t HmwDevice::hardwareVersion( 0 );

uint16_t HmwDevice::firmwareVersion( 0 );

uint32_t HmwDevice::ownAddress( 0x60000000 );

HmwDevice::BasicConfig* HmwDevice::basicConfig( 0 );

// default constructor
HmwDevice::HmwDevice()
{
} // HmwDevice

