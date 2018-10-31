/*
 * RS485Hw.h
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */



#ifndef HW_PROTOCOLS_RS485HW_H_
#define HW_PROTOCOLS_RS485HW_H_

#include "HwProtocols.h"
#include "Peripherals/Usart.h"
#include "DigitalOutput.h"
#include <IoStream.h>

class RS485Hw : public IoStream
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
         user( NULL ),
         disableRx( _enableRx ),
         enableTx( _enableTx ),
         pendingEscape( false ),
         rxMsgComplete( false ),
         rxBufferOverflow( false ),
         transmissionPending( false )
      {

      }
      ////    Operations    ////

   public:

      Stream::Status genericCommand( Command command, void* buffer );

      Stream::Status read( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

      Stream::Status write( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

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

      volatile uint16_t receiveBufferSize;

      uint16_t maxReceiveBufferSize;

      Usart* usart;

      EventDrivenUnit* user;

      DigitalOutput disableRx;

      DigitalOutput enableTx;

      bool pendingEscape;

      volatile bool rxMsgComplete;

      volatile bool rxBufferOverflow;

      volatile bool transmissionPending;

      static const uint8_t debugLevel;

};

#endif /* HWPROTOCOLS_RS485HW_H_ */
