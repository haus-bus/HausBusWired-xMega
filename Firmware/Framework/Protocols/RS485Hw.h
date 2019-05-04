/*
 * RS485Hw.h
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */



#ifndef HW_PROTOCOLS_RS485HW_H_
#define HW_PROTOCOLS_RS485HW_H_

#include "Protocols.h"

#include <DigitalOutput.h>
#include <Time/Timestamp.h>
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
         rxBufferOverflow( false )
      {

      }
      ////    Operations    ////

   public:

      IStream::Status genericCommand( Command command, void* buffer );

      IStream::Status read( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

      IStream::Status write( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

      void handleDataRegisterEmpty();

      void handleTransmitFinished();

      bool handleDataReceivedFromISR();

      inline void notifyRxStartFromISR()
      {
         transmissionStartTime = Timestamp();
      }

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

      Timestamp transmissionStartTime;

      volatile bool rxMsgComplete;

      volatile bool rxBufferOverflow;

      static const uint8_t debugLevel;

};

#endif /* HWPROTOCOLS_RS485HW_H_ */
