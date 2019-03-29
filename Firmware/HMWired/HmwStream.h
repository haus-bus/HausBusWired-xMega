/*
 * HmWStream.h
 *
 * Created: 18.11.2017 17:32:07
 * Author: viktor.pankraz
 */


#ifndef __HMWSTREAM_H__
#define __HMWSTREAM_H__

#include "HmwMessageBase.h"
#include "HmwStreamBase.h"

#include <Containers/StaticQueue.h>
#include <Containers/ArrayVector.h>

class HmwStream : public HmwStreamBase
{
// variables
   public:

      static const uint8_t MESSAGE_QUEUE_SIZE = 8;
      static const uint8_t ARRAY_VECTOR_SIZE = 16;

      typedef StaticQueue<HmwMessageBase*, uint8_t, MESSAGE_QUEUE_SIZE> MessageQueue;
      typedef ArrayVector<HmwMessageBase*, ARRAY_VECTOR_SIZE> MessageVector;

   protected:
   private:

// functions
   public:

      static inline void setHardware( HmwStreamHw* _hardware )
      {
         hardware = _hardware;
         hardware->serial->init<19200, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc, USART_CHSIZE_8BIT_gc, false, false>();
         hardware->serial->enableReceiveCompleteInterrupt();
      }

      static IStream::Status sendMessage( HmwMessageBase& msg );

      static inline HmwMessageBase* getMessage()
      {
         HmwMessageBase* msg = NULL;
         if ( !inMessageQueue.isEmpty() )
         {
            CriticalSection doNotInterrupt;
            inMessageQueue.pop( msg );
         }
         return msg;
      }

      static void handlePendingOutMessages()
      {
         for ( uint8_t i = 0; i < outMessageVector.size(); i++ )
         {
            HmwMessageBase* outMsg = outMessageVector.getElement( i );
            const Timestamp& nextSendTime = outMsg->getNextSendTime();
            if ( nextSendTime.isValid() && nextSendTime.since() )
            {
               sendMessage( *outMsg );
               if ( !outMsg->hasSendingTriesLeft() )
               {
                  // all retires were done, remove message from vector
                  outMessageVector.remove( outMsg );
                  delete outMsg;
               }
               return;
            }
         }
      }

      static inline void notifyReceivedAckOrInfoLevel( const HmwMessageBase& inMsg )
      {
         for ( uint8_t i = 0; i < outMessageVector.size(); i++ )
         {
            HmwMessageBase* outMsg = outMessageVector.getElement( i );
            if ( outMsg->isAcknowledgedBy( inMsg ) )
            {
               outMessageVector.remove( outMsg );
               delete outMsg;
               return;
            }
         }
      }

      static inline bool nextByteReceivedFromISR( uint8_t data )
      {
         // this function is not secured because it should be called from ISR only
         HmwMessageBase* msg = nextByteReceived( data );
         if ( msg )
         {
            if ( !msg->isFromMe() )
            {
               HmwMessageBase* newMsg = inMessage.copy();
               if ( newMsg )
               {
                  inMessageQueue.push( newMsg );
               }
            }
            return true;
         }
         return false;
      }

   protected:

   private:

      static MessageQueue inMessageQueue;

      static MessageVector outMessageVector;

}; // HmWStream

#endif // __HMWSTREAM_H__
