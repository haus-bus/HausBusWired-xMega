/*
 * HmWStreamBase.h
 *
 * Created: 13.12.2017 17:32:07
 * Author: viktor.pankraz
 */


#ifndef __HMWSTREAMBASE_H__
#define __HMWSTREAMBASE_H__

#include "HmwMessageBase.h"
#include "HmwStreamHw.h"

class HmwStreamBase
{
// variables
   public:
      static const uint8_t MIN_IDLE_TIME = 7;

   protected:

      static const uint8_t debugLevel;

      static uint8_t senderNum;

      static uint8_t receiverNum;

      static Timestamp lastReceivedTime;

      static HmwMessageBase inMessage;

      static HmwStreamHw* hardware;

   private:

// functions
   public:

      static inline void setHardware( HmwStreamHw* _hardware )
      {
         hardware = _hardware;
         hardware->serial->init<19200, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc, USART_CHSIZE_8BIT_gc, false, false>();
      }

      static IStream::Status sendMessage( HmwMessageBase& msg );

      static HmwMessageBase* pollMessageReceived();

      static inline bool isIdle()
      {
      #ifdef _BOOTER_
         uint8_t randomDelay = MIN_IDLE_TIME;
      #else
         uint8_t randomDelay = ( rand() & 0xF ) + MIN_IDLE_TIME;
         CriticalSection doNotInterrupt;
      #endif
         return lastReceivedTime.since() > randomDelay;
      }

      static inline void notifyRxStartFromISR()
      {
         lastReceivedTime = Timestamp();
      }

      static inline void sync( uint8_t _receiverNum )
      {
         senderNum = 0;
         receiverNum = _receiverNum;
      }

      static inline void notifyMsgReceived( const uint32_t& senderAddress )
      {
         receiverNum++;
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

      static HmwMessageBase* nextByteReceived( uint8_t data );

      static bool getNextByteToSend( uint8_t& data );

   private:


}; // HmWStreamBase

#endif // __HMWSTREAMBASE_H__
