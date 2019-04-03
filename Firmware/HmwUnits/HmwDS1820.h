/*
 * HBWDS1820.h
 *
 *  Created on: 26.04.2017
 *      Author: Viktor Pankraz
 */

#ifndef HmwDS1820_H
#define HmwDS1820_H

#include <Protocols/OneWire.h>
#include <Time/Timestamp.h>
#include <xEeprom.h>

#include "HmwChannel.h"


class HmwDS1820 : public HmwChannel
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
         SEND_FEEDBACK,
         SEND_INVALID_VALUE
      };

      struct Config
      {
         uint8_tx id;
         uint8_tx minDelta;
         uint16_tx minInterval;
         uint16_tx maxInterval;
      };

      static const uint8_t SCRATCHPAD_SIZE = 9;

      static const uint8_t DS18B20_CONF_REG = 4;
      static const uint8_t DS18B20_9_BIT = 0;
      static const uint8_t DS18B20_10_BIT = ( 1 << 5 );
      static const uint8_t DS18B20_11_BIT = ( 1 << 6 );
      static const uint8_t DS18B20_12_BIT = ( ( 1 << 6 ) | ( 1 << 5 ) );

      // indefined bits in LSB if 18B20 != 12bit
      static const uint8_t DS18B20_9_BIT_UNDF = ( ( 1 << 0 ) | ( 1 << 1 ) | ( 1 << 2 ) );
      static const uint8_t DS18B20_10_BIT_UNDF = ( ( 1 << 0 ) | ( 1 << 1 ) );
      static const uint8_t DS18B20_11_BIT_UNDF = ( ( 1 << 0 ) );
      static const uint8_t DS18B20_12_BIT_UNDF = 0;

      // family ids
      static const uint8_t DS18S20_ID = 0x10;
      static const uint8_t DS18B20_ID = 0x28;

      ////    Constructors and destructors    ////

      HmwDS1820( OneWire& _hardware, Config* _config );

      ////    Operations    ////

      bool isSelfPowered();

      static bool isSensor( uint8_t familiyCode );

      HwStatus readMeasurement();

      HwStatus startMeasurement( bool allSensors = true );

      // definition of needed functions from HBWChannel class
      virtual uint8_t get( uint8_t* data );
      virtual void loop( uint8_t channel );
      virtual void checkConfig();

   private:

      int16_t convertToCentiCelsius( uint8_t* scratchPad );

      ////    Additional operations    ////

   public:

      static bool isUsed( uint8_t id );

      inline OneWire* getHardware() const
      {
         return hardware;
      }

      inline static bool getSelfPowered()
      {
         return selfPowered;
      }

      inline static void setSelfPowered( bool p_selfPowered )
      {
         selfPowered = p_selfPowered;
      }

      inline uint8_t getConfigId()
      {
         return config->id;
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

      OneWire* hardware;

   private:

      static const uint8_t debugLevel;

      Config* config;

      State state;

      OneWire::RomCode romCode;

      int16_t currentCentiCelsius;

      int16_t lastSentCentiCelsius;

      Timestamp lastActionTime;

      Timestamp lastSentTime;
};


#endif

