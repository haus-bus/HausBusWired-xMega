/*
 * RS485Hw.h
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */

#ifndef HWPROTOCOLS_RS485HW_H_
#define HWPROTOCOLS_RS485HW_H_

#include "HwProtocols.h"
#include "Peripherals/Twi.h"
#include "DigitalOutput.h"
#include <IoStream.h>

class TwiHw : public IoStream
{
   public:

      ////    Constructors and destructors    ////

      inline TwiHw( Twi& _twi, DigitalOutput _scl,
                    DigitalOutput _sda ) :
         owner( NULL ), transmitBuffer( NULL ), receiveBuffer( NULL ),
         receiveBufferSize( 0 ), maxReceiveBufferSize( 0 ), twi( &_twi ),
         scl( _scl ), sda( _sda )
      {

      }
      ////    Operations    ////

   public:

      virtual Stream::Status genericCommand( Command command, void* buffer );

      virtual Stream::Status read( void* pData, uint16_t length,
                                   EventDrivenUnit* user = 0 );

      virtual Stream::Status write( void* pData, uint16_t length,
                                    EventDrivenUnit* user = 0 );

      void handleSlaveStatus();

      void handleMasterStatus();

   protected:

      void notifyEndOfSlaveTransfer();

      void notifyEndOfmasterTransfer();

   private:

      ////    Attributes    ////

   protected:

      EventDrivenUnit* owner;

      uint8_t* transmitBuffer;

      uint8_t* receiveBuffer;

      uint16_t receiveBufferSize;

      uint16_t maxReceiveBufferSize;

      Twi* twi;

      DigitalOutput scl;

      DigitalOutput sda;

      static const uint8_t debugLevel;

};

#endif /* HWPROTOCOLS_RS485HW_H_ */
