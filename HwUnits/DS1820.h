/*
 * DS1820.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_DS1820_H
#define HwUnits_DS1820_H

#include "HwUnits.h"
#include "BaseSensorUnit.h"
#include <OneWire.h>
class Crc8;

class Event;

class Scheduler;

class DS1820 : public BaseSensorUnit
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

      DS1820( const OneWire& _hardware, const OneWire::RomCode& _romCode );

      ////    Operations    ////

      bool isSelfPowered();

      static bool isSensor( uint8_t familiyCode );

      static void scanAndCreateDevices( PortPin _owPin );

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size );

      HwStatus readMeasurement();

      void run();

      HwStatus startMeasurement( bool allSensors = true );

   private:

      BaseSensorUnit::Status convertToCelsius( uint8_t* scratchPad );

      ////    Additional operations    ////

   public:

      inline OneWire* getHardware() const
      {
         return (OneWire*) &hardware;
      }

      inline static uint8_t getNumOfInstances()
      {
         return numOfInstances;
      }

      inline static void setNumOfInstances( uint8_t p_numOfInstances )
      {
         numOfInstances = p_numOfInstances;
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

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

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

      OneWire hardware;

      static uint8_t numOfInstances;

      static bool selfPowered;

   private:

      static const uint8_t debugLevel;

      OneWire::RomCode romCode;

};

inline void* DS1820::operator new( size_t size )
{
   return allocOnce( size );
}

#endif

