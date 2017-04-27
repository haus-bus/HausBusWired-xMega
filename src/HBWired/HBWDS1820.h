/*
 * HBWDS1820.h
 *
 *  Created on: 26.04.2017
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_DS1820_H
#define HwUnits_DS1820_H

#include <Protocols/OneWire.h>
#include <Utils/Timestamp.h>

#include "HBWired.h"

class Crc8;

class Event;

class Scheduler;

class HBWDS1820 : public HBWChannel
{
public:

  static const uint8_t CONVERT_T = 0x44;
  static const uint8_t READ_POWER_SUPPLY = 0xB4;
  static const uint8_t READ = 0xBE;
  static const uint8_t WRITE = 0x4E;
  static const uint8_t WRITE_EE = 0x48;
  static const uint8_t RECALL_EE = 0xB8;

  enum HwStatus
  {
    OK,
    START_FAIL,
    FAILTURE,
    CRC_FAILTURE,
    OUT_OF_MEMORY
  };

    enum State
    {
        SEARCH_SENSOR,   
        START_MEASUREMENT,
        SEND_FEEDBACK
    };

  	struct Config
  	{
      	uint8_t     unused1;           
      	uint8_t     minDelta;
        uint8_t     unused2;
        uint16_t    minInterval;
        uint16_t    maxInterval;
        uint8_t     unused3[9]; 
  	};

  static const uint8_t SCRATCHPAD_SIZE = 9;

  static const uint8_t DS18B20_CONF_REG = 4;
  static const uint8_t DS18B20_9_BIT = 0;
  static const uint8_t DS18B20_10_BIT = (1 << 5);
  static const uint8_t DS18B20_11_BIT = (1 << 6);
  static const uint8_t DS18B20_12_BIT = ((1 << 6) | (1 << 5));

  // indefined bits in LSB if 18B20 != 12bit
  static const uint8_t DS18B20_9_BIT_UNDF = ((1 << 0) | (1 << 1) | (1 << 2));
  static const uint8_t DS18B20_10_BIT_UNDF = ((1 << 0) | (1 << 1));
  static const uint8_t DS18B20_11_BIT_UNDF = ((1 << 0));
  static const uint8_t DS18B20_12_BIT_UNDF = 0;

  // family ids
  static const uint8_t DS18S20_ID = 0x10;
  static const uint8_t DS18B20_ID = 0x28;

  ////    Constructors and destructors    ////

  HBWDS1820( const OneWire& _hardware, Config* _config );

  ////    Operations    ////

  bool isSelfPowered();

  static bool isSensor( uint8_t familiyCode );
  
  HwStatus readMeasurement();

  HwStatus startMeasurement( bool allSensors = true );

    // definition of needed functions from HBWChannel class
    virtual uint8_t get(uint8_t* data);
    virtual void loop(HBWDevice*, uint8_t channel);

private:

  int16_t convertToCentiCelsius( uint8_t* scratchPad );

  ////    Additional operations    ////

public:

  inline OneWire* getHardware() const
  {
    return (OneWire*) &hardware;
  }

  inline static bool getSelfPowered()
  {
    return selfPowered;
  }

  inline static void setSelfPowered( bool p_selfPowered )
  {
    selfPowered = p_selfPowered;
  }

private:

  inline OneWire::RomCode getRomCode() const
  {
    return romCode;
  }

  inline void setRomCode( OneWire::RomCode p_romCode )
  {
    romCode = p_romCode;
  }

  ////    Attributes    ////

public:

  static bool selfPowered;

  OneWire hardware;

private:


    Config* config;

    State state;

    OneWire::RomCode romCode;

    uint16_t nextActionDelay;

    int16_t currentCentiCelsius;

    int16_t lastSentCentiCelsius;

    Timestamp lastActionTime;
    
    Timestamp lastSentTime;

    
    

};


#endif

