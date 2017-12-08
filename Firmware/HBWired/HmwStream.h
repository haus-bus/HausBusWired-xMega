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

#include <Containers/StaticQueue.h>

class HmwStream
{
// variables
   public:

      static const uint8_t MESSAGE_QUEUE_SIZE = 16;
      static const uint8_t MAX_RETRIES = 3;
      static const uint8_t RETRY_DELAY_TIME = 100;
      static const uint8_t MIN_IDLE_TIME = 7;

      typedef StaticQueue<HmwMessageBase*, uint8_t, MESSAGE_QUEUE_SIZE> MessageQueue;

   protected:
   private:

// functions
   public:

      static inline void setHardware( HmwStreamHw* _hardware, MessageQueue* inQueue = NULL )
      {
         hardware = _hardware;
         hardware->serial->init<19200, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc, USART_CHSIZE_8BIT_gc, false, false>();
      }

      static inline void setInMessageQueue( MessageQueue* queue )
      {
         inMessageQueue = queue;
         if ( inMessageQueue )
         {
            hardware->serial->enableReceiveCompleteInterrupt();
         }
         else
         {
            hardware->serial->disableReceiveCompleteInterrupt();
         }
      }

      static Stream::Status sendMessage( HmwMessageBase& msg );

      static HmwMessageBase* pollMessageReceived();

      static inline HmwMessageBase* getMessageFromQueue()
      {
         HmwMessageBase* msg = NULL;
         if ( inMessageQueue && !inMessageQueue->isEmpty() )
         {
            CriticalSection doNotInterrupt;
            inMessageQueue->pop( msg );
         }
         return msg;
      }

      static inline bool isIdle()
      {
         // toDo
         return lastReceivedTime.since() > MIN_IDLE_TIME;
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

      static void nextByteReceivedFromISR( uint8_t data )
      {
         // this function is not secured because it should be called from ISR only
         HmwMessageBase* msg = nextByteReceived( data );
         if ( msg && inMessageQueue )
         {
            HmwMessageBase* msg = inMessage.copy();
            inMessageQueue->push( msg );
         }
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

      static const uint8_t debugLevel;

      static uint8_t senderNum;

      static uint8_t receiverNum;

      static Timestamp lastReceivedTime;

      static HmwMessageBase inMessage;

      static HmwStreamHw* hardware;

      static MessageQueue* inMessageQueue;

}; // HmWStream

#endif // __HMWSTREAM_H__
