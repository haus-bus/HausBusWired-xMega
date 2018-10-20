/*
 * RS485Hw.h
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */

#ifndef HWPROTOCOLS_RS485HW_H_
#define HWPROTOCOLS_RS485HW_H_

#include "HwProtocols.h"
#include "Peripherals/Usart.h"
#include "DigitalOutput.h"
#include <Stream.h>

class RS485Hw
{
   public:

      enum Consts
      {
         FRAME_STARTBYTE = 0xFD,
         FRAME_STOPBYTE = 0xFE,
         ESCAPE_BYTE = 0xFC
      };

      ////    Constructors and destructors    ////

      inline RS485Hw( Usart& _usart, DigitalOutput _enableRx, DigitalOutput _enableTx ) :
         transmitBuffer( NULL ), 
         receiveBuffer( NULL ),
         receiveBufferSize( 0 ), 
         maxReceiveBufferSize( 0 ), 
         usart( &_usart ),
         disableRx( _enableRx ), 
         enableTx( _enableTx ),
         pendingEscape( false )
      {

      }
      ////    Operations    ////

   public:

      Stream::Status init( void* buffer, uint16_t buffersize );

      uint16_t read( void* pData, uint16_t length );

      uint16_t write( void* pData, uint16_t length );

      void handleDataRegisterEmpty();

      void handleTransmitFinished();

      void handleDataReceived();

   protected:

      bool init();

      inline void disableReceiver()
      {
         disableRx.set();
      }

      inline void enableReceiver()
      {
         disableRx.clear();
      }

      inline bool isReceiverEnabled()
      {
         return !disableRx.isSet();
      }

      inline void disableTransmitter()
      {
         enableTx.clear();
      }

      inline void enableTransmitter()
      {
         enableTx.set();
      }

      inline bool isTransmitterEnabled()
      {
         return enableTx.isSet();
      }

   private:

      ////    Attributes    ////

   protected:

      uint8_t* transmitBuffer;

      uint8_t* receiveBuffer;

      uint16_t receiveBufferSize;

      uint16_t maxReceiveBufferSize;

      Usart* usart;

      DigitalOutput disableRx;

      DigitalOutput enableTx;

      bool pendingEscape;

      static const uint8_t debugLevel;

};

#endif /* HWPROTOCOLS_RS485HW_H_ */
