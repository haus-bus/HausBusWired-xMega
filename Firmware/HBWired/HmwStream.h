/*
 * HmWStream.h
 *
 * Created: 18.11.2017 17:32:07
 * Author: viktor.pankraz
 */


#ifndef __HMWSTREAM_H__
#define __HMWSTREAM_H__

#include "HmwMessageBase.h"
#include <Peripherals/Usart.h>

class HmwStream
{
// variables
   public:
   protected:
   private:

// functions
   public:

      static inline void setStream( Usart& _usart )
      {
         usart = &_usart;
         usart->init<19200, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc, USART_CHSIZE_8BIT_gc, true, false>();
      }

      static Stream::Status sendMessage( HmwMessageBase* msg );

      static HmwMessageBase* pollMessageReceived();

      static inline bool isIdle()
      {
         // toDo random number
         return lastReceivedTime.since() > ( 100 );
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

      static Usart* usart;


}; // HmWStream

#endif // __HMWSTREAM_H__
