/*
 * HmWStream.h
 *
 * Created: 18.11.2017 17:32:07
 * Author: viktor.pankraz
 */


#ifndef __HMWSTREAM_H__
#define __HMWSTREAM_H__

#include "HmwMessageBase.h"
#include "HmwStreamHw.h"

class HmwStream
{
// variables
   public:
   protected:
   private:

// functions
   public:

      static inline void setHardware( HmwStreamHw* _hardware )
      {
         hardware = _hardware;
         hardware->serial->init<19200, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc, USART_CHSIZE_8BIT_gc, false, false>();
         /*
            hardware->serial->setBaudrate( 106, 0 ); // 19185 bps at 32MHz, clock not doubled
            hardware->serial->setFormat( USART_CMODE_ASYNCHRONOUS_gc, USART_CHSIZE_8BIT_gc, USART_PMODE_EVEN_gc, false );
            hardware->serial->enableTransmitter();
            hardware->serial->enableReceiver();
          */
      }

      static Stream::Status sendMessage( HmwMessageBase* msg );

      static HmwMessageBase* pollMessageReceived();

      static inline bool isIdle()
      {
         // toDo
         return lastReceivedTime.since() > 7;
      }

   protected:

      struct CommunicationStatus
      {
         HmwMessageBase* msg;
         uint16_t crc16checksum;
         uint8_t dataIdx;
         uint8_t transmitRetry;
         uint8_t pendingEscape : 1;
         uint8_t transmitting  : 1;
      };

      static CommunicationStatus statusSending, statusReceiving;

      static bool nextByteReceived( uint8_t data );

      static bool getNextByteToSend( uint8_t& data );

   private:

      static const uint8_t debugLevel;

      static uint8_t senderNum;

      static uint8_t receiverNum;

      static Timestamp lastReceivedTime;

      static HmwStreamHw* hardware;


}; // HmWStream

#endif // __HMWSTREAM_H__
