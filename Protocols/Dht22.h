/*
 * Dht22.h
 *
 *  Created on: 09.05.2015
 *      Author: viktor.pankraz
 */

#ifndef HwProtocols_Dht22_H
#define HwProtocols_Dht22_H

#include "Protocols.h"

#include <Time/SystemTime.h>
#include <PortPin.h>

class Dht22
{
   public:

      static const uint8_t BYTE_COUNT = 5;
      static const uint8_t IDLE_TIMEOUT = 250;
      static const uint8_t ACK_TIMEOUT = 100;

      enum Errors
      {
         OK,
         BUS_HUNG,
         NOT_PRESENT,
         ACK_TOO_LONG,
         SYNC_TIMEOUT,
         DATA_TIMEOUT,
         CHECKSUM_ERROR,
         ACK_MISSING
      };

      ////    Constructors and destructors    ////

      Dht22( PortPin pin );

      ////    Operations    ////

      uint8_t read( uint8_t* data );

   private:

      inline void reset();

      inline Errors waitForAck();

      inline bool waitForIdle();

      ////    Additional operations    ////

   public:

      inline PortPin* getPortPin() const
      {
         return (PortPin*) &portPin;
      }

   private:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      ////    Attributes    ////

   public:

      PortPin portPin;

   private:

      static const uint8_t debugLevel;
};

inline void Dht22::reset()
{
   DEBUG_M1( FSTR( "reset" ) );
   IoPort& ioPort = portPin.getIoPort();
   uint8_t pin = portPin.getPin();

   {
      CriticalSection doNotInterrupt;
      ioPort.clearPins( pin );
      ioPort.setPinsAsOutput( pin );
   }
   SystemTime::waitMs( 18 );
}

inline Dht22::Errors Dht22::waitForAck()
{
   CriticalSection doNotInterrupt;
   uint8_t pin = portPin.getPin();
   uint8_t* port = ( (uint8_t*) &portPin.getIoPort() ) + 8;

   portPin.getIoPort().setPinsAsInput( pin );

   // find the start of the ACK signal
   uint8_t remaining = delayBit( COUNT_DELAY_BIT_US( ACK_TIMEOUT / 2 ), port, pin,
                                 1 );
   if ( !remaining )
   {
      DEBUG_M1( FSTR( "waitForAck not present" ) );
      return NOT_PRESENT;
   }

   // find the transition of the ACK signal
   remaining = delayBit( COUNT_DELAY_BIT_US( ACK_TIMEOUT ), port, pin, 0 );
   if ( !remaining )
   {
      DEBUG_M1( FSTR( "waitForAck 0 too long" ) );
      return ACK_MISSING;
   }

   // find the end of the ACK signal
   remaining = delayBit( COUNT_DELAY_BIT_US( ACK_TIMEOUT ), port, pin, 1 );
   if ( !remaining )
   {
      DEBUG_M1( FSTR( "waitForAck 1 too long" ) );
      return ACK_TOO_LONG;
   }

   // find the end of the ACK signal
   remaining = delayBit( COUNT_DELAY_BIT_US( ACK_TIMEOUT ), port, pin, 0 );
   if ( !remaining )
   {
      DEBUG_M1( FSTR( "waitForDataSync too long" ) );
      return SYNC_TIMEOUT;
   }
   return OK;
}

inline bool Dht22::waitForIdle()
{
   DEBUG_M1( FSTR( "wairForIdle" ) );
   uint8_t* port = ( (uint8_t*) &portPin.getIoPort() ) + 8;
   uint8_t pin = portPin.getPin();

   // wait if data line is not idle for max 250uSec
   return delayBit( COUNT_DELAY_BIT_US( IDLE_TIMEOUT ), port, pin, 1 );
}

#endif
