/*
 * HMWModule.h
 *
 *  Created on: 09.05.2014
 *      Author: thorsten
 */

#ifndef HMWMODULE_H_
#define HMWMODULE_H_

#include "HMWRS485.h"

// TODO: Firmware/Hardware Version aus EEPROM bzw. Flash?
#define MODULE_HARDWARE_VERSION 1
#define MODULE_FIRMWARE_VERSION 0x0306

// Abstrakte Basisklasse mit Callbacks aus dem Modul
class HMWDeviceBase
{
public:
  virtual void setLevel( uint8_t, unsigned int ) = 0;  // channel, level
  virtual unsigned int getLevel( uint8_t ) = 0;       // channel, returns level
  virtual void readConfig() = 0;         // read config from EEPROM
};

class HMWModule: public HMWModuleBase
{
public:
  HMWModule( HMWDeviceBase*, HMWRS485*, uint8_t ); // rs485, device type
  virtual ~HMWModule();

  virtual void processEvent( uint8_t const * const frameData, uint8_t frameDataLength,
                             bool isBroadcast = false );

  void broadcastAnnounce( uint8_t );  // channel
  void broadcastKeyEvent( uint8_t, uint8_t, uint8_t = 0 ); // channel, keyPressNum, long/short (long = 1)
  void sendInfoMessage( uint8_t, uint8_t *ptr, uint8_t length, unsigned long ); // channel, info, target address

  uint8_t deviceType;      // device type @ 0x7FF1 in FlashRom  TODO: Not really...

  // write to EEPROM, but only if not "value" anyway
  // the uppermost 4 bytes are reserved for the device address and can only be changed if privileged = true
  void writeEEPROM( int address, uint8_t value, bool privileged = false );
  void setNewId();

private:
  HMWRS485* hmwrs485;
  HMWDeviceBase* device;

  void readAddressFromEEPROM();
  void determineSerial( uint8_t* );

  void processEventKey();
  void processEventSetLevel( uint8_t channel, unsigned int level );
  void processEventGetLevel( uint8_t channel );
  void processEventSetLock();
  void processEmessage( uint8_t const * const frameData );
};

#endif /* HMWMODULE_H_ */
