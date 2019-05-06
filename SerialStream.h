/*
 * CDCStream.h
 *
 * Created: 21.04.2017 00:18:42
 * Author: Viktor Pankraz
 */


#ifndef __SERIALSTREAM_H__
#define __SERIALSTREAM_H__

#include <IoStream.h>
#include <Peripherals/Usart.h>
#include <PortPin.h>

class SerialStream : public IoStream
{
// variables
   public:
   protected:
   private:
      Usart* serial;

// functions
   public:
      SerialStream( Usart*, PortPin _rx, PortPin _tx );

      template<uint32_t baudrate, USART_CMODE_t mode = USART_CMODE_ASYNCHRONOUS_gc,
               USART_PMODE_t parity = USART_PMODE_DISABLED_gc, USART_CHSIZE_t characterSize = USART_CHSIZE_8BIT_gc,
               bool twoStopBits = false, bool doubleClock = true>
      inline bool init()
      {
         return serial->init<baudrate, mode, parity, characterSize, twoStopBits, doubleClock>();
      }

      virtual IStream::Status genericCommand( IoStream::Command command, void* data );

      virtual IStream::Status read( uint8_t& data );

      virtual IStream::Status write( uint8_t data );

      virtual bool available();

   protected:
   private:


}; // SerialStream

#endif // __SERIALSTREAM_H__
