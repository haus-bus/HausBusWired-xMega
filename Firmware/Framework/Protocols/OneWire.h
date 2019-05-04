/*
 * OneWire.h
 *
 *  Created on: 24.06.2014
 *      Author: viktor.pankraz
 */

#ifndef HwProtocols_OneWire_H
#define HwProtocols_OneWire_H

#include "Protocols.h"
#include <PortPin.h>

class DS1820;

class PortPin;


class OneWire
{
   public:

      static const uint8_t RECOVERY_DELAY = 2;
      static const uint8_t CYCLES_PER_IO_ACCESS = 13;
      static const uint8_t IO_ACCESS_DELAY = CYCLES_PER_IO_ACCESS * 1000000L / F_CPU;
      static const uint8_t READ_TIME_SLOT = 15;
      static const uint8_t BIT_TIME_SLOT = 60;

      static const uint8_t ROMCODE_SIZE = 8;
      static const uint8_t LAST_DEVICE = 0x00;
      static const uint8_t SEARCH_FIRST = 0xFF;


      static const uint8_t DATA_ERROR = 0xFE;
      static const uint8_t PRESENCE_ERROR = 0xFF;

      static const uint8_t MATCH_ROM = 0x55;
      static const uint8_t SKIP_ROM = 0xCC;
      static const uint8_t SEARCH_ROM = 0xF0;


      struct RomCode {
         uint8_t family;
         uint8_t serial[6];
         uint8_t crc;
      };

      ////    Constructors and destructors    ////

      OneWire( PortPin portPin );

      ////    Operations    ////

      inline void disableParasite();

      inline void enableParasite();

      inline uint8_t isIdle();

      inline uint8_t read();

      uint8_t reset();

      void scanAndCreateDevices();

      uint8_t searchROM( uint8_t diff, uint8_t* id );

      void sendCommand( uint8_t command, uint8_t* id );

      // Timing issue when using runtime-bus-selection (!OW_ONE_BUS):
      // The master should sample at the end of the 15-slot after initiating
      // the read-time-slot. The variable bus-settings need more
      // cycles than the constant ones so the delays had to be shortened
      // to achive a 15uS overall delay
      // Setting/clearing a bit in I/O Register needs 1 cyle in OW_ONE_BUS
      // but around 14 cyles in configureable bus (us-Delay is 4 cyles per uS)
      uint8_t sendReceiveBit( uint8_t bit );

      uint8_t write( uint8_t data );

      ////    Additional operations    ////

      inline IoPort& getIoPort() const
      {
         return *ioPort;
      }

      inline void setIoPort( IoPort& p_ioPort )
      {
         ioPort = &p_ioPort;
      }

      inline uint8_t getPin() const
      {
         return pin;
      }

      inline void setPin( uint8_t p_pin )
      {
         pin = p_pin;
      }

   private:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      ////    Attributes    ////

   public:
      uint8_t pin;

      IoPort* ioPort;


   private:

      static const uint8_t debugLevel;
};

inline void OneWire::disableParasite()
{
   ioPort->clearPins( pin );
   ioPort->setPinsAsInput( pin );
}

inline void OneWire::enableParasite()
{
   ioPort->setPins( pin );
   ioPort->setPinsAsOutput( pin );
}

inline uint8_t OneWire::isIdle()
{
   return ioPort->isPinSet( pin );
}

inline uint8_t OneWire::read()
{
   return write( 0xFF );
}

#endif
