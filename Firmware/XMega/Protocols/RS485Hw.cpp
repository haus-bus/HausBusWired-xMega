/*
 * RS485Hw.cpp
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */

#include <Protocols/RS485Hw.h>
#include <EventPkg/EventPkg.h>
#include <Peripherals/Usart.h>

#define getId() FSTR( "RS485Hw::" )
const uint8_t RS485Hw::debugLevel( TRACE_PORT | DEBUG_LEVEL_OFF );

enum TracePins
{
   RX_INT = Pin0,
   RX_MSG = Pin1,
   RX_ESC = Pin2,
   RX_COLL = Pin3,
   RX_READ = Pin4,
   TX_MSG = Pin5,
   TX_ERR = Pin6

};

bool RS485Hw::handleDataReceivedFromISR()
{
   TRACE_PORT_SET( RX_INT );
   uint8_t data;
   usart->read( data );

   if ( data == FRAME_STARTBYTE )
   {
      TRACE_PORT_SET( RX_MSG );
      TRACE_PORT_CLEAR( RX_COLL );
      if ( rxMsgComplete )
      {
         rxBufferOverflow = true;
         rxMsgComplete = false;
      }
      receiveBufferSize = 0;
      transmissionStartTime.setNow();
   }
   else if ( data == FRAME_STOPBYTE )
   {
      TRACE_PORT_CLEAR( RX_MSG );
      // message completed
      if ( transmitBuffer == NULL )
      {
         rxMsgComplete = true;
      }
      transmissionStartTime.reset();
   }
   else if ( data == ESCAPE_BYTE )
   {
      TRACE_PORT_SET( RX_ESC );
      pendingEscape = true;
   }
   else
   {
      // restore data first
      if ( pendingEscape )
      {
         data |= 0x80;
         pendingEscape = false;
         TRACE_PORT_CLEAR( RX_ESC );
      }
      if ( transmitBuffer )
      {
         if ( transmitBuffer[receiveBufferSize] != data )
         {
            // notify collision
            disableTransmitter();
            TRACE_PORT_SET( RX_COLL );
         }
      }
      receiveBuffer[receiveBufferSize] = data;
      if ( receiveBufferSize < maxReceiveBufferSize )
      {
         receiveBufferSize++;
      }
      else
      {
         // notify buffer overrun
      }
   }
   TRACE_PORT_CLEAR( RX_INT );
   return transmissionStartTime.isValid();
}

bool RS485Hw::init()
{
   bool hasError;
   if ( F_CPU == 32000000 )
   {
      hasError = usart->init<1097, -5, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc>();
   }
   else if ( F_CPU == 8000000 )
   {
      hasError = usart->init<1001, -7, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc>();
   }
   else
   {
      hasError = usart->init<57600, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc>();
   }
   return hasError;
}

IStream::Status RS485Hw::genericCommand( IoStream::Command command, void* buffer )
{
   if ( command == IoStream::INIT )
   {
      DEBUG_H1( FSTR( "init..." ) );
      IoStream::CommandINIT* cmd = (IoStream::CommandINIT*) buffer;

      receiveBuffer = new uint8_t[cmd->buffersize];

      if ( !receiveBuffer )
      {
         return IStream::Status::INVALID_BUFFER;
      }
      maxReceiveBufferSize = cmd->buffersize;

      DEBUG_M2( FSTR( "bufferSize: 0x" ), maxReceiveBufferSize );
      bool error = init();
      if ( error )
      {
         ERROR_1( FSTR( "hardware init failed!" ) )
         return IStream::Status::ABORTED;
      }
      usart->enableReceiveCompleteInterrupt();

      return IStream::Status::SUCCESS;
   }
   return IStream::Status::ABORTED;
}

IStream::Status RS485Hw::read( void* pData, uint16_t length, EventDrivenUnit* user )
{
   IStream::TransferDescriptor td;
   {
      CriticalSection doNotInterrupt;
      if ( rxBufferOverflow )
      {
         rxBufferOverflow = false;
         return IStream::BUFFER_OVERFLOW;
      }
      if ( rxMsgComplete )
      {
         TRACE_PORT_SET( RX_READ );
         memcpy( pData, receiveBuffer, receiveBufferSize );
         td.setBytesTransferred( receiveBufferSize );
         td.setPData( (uint8_t*)pData );
         rxMsgComplete = false;
      }
   }
   if ( td.pData )
   {
      if ( user )
      {
         evData( user, &td ).send();
      }
      TRACE_PORT_CLEAR( RX_READ );
      return IStream::SUCCESS;
   }
   return IStream::NO_DATA;
}

IStream::Status RS485Hw::write( void* pData, uint16_t length, EventDrivenUnit* user )
{
   TRACE_PORT_SET( TX_MSG );
   DEBUG_H3( FSTR( "sending 0x" ), length, FSTR( " Bytes" ) );
   // critical section needed because accessing transmissionStartTime that can be modified in an interrupt
   {
      CriticalSection doNotInterrupt;
      if ( rxMsgComplete || ( transmissionStartTime.isValid() && ( transmissionStartTime.since() < 50 ) ) )
      {
         // read first the incoming message because receiveBuffer will be overwritten with transmitting new message
         return IStream::LOCKED;
      }
      enableTransmitter();
   }

   transmitBuffer = (uint8_t*) pData;
   uint16_t transmitIdx = 0;
   usart->write( FRAME_STARTBYTE );

   while ( isTransmitterEnabled() && ( transmitIdx < length ) )
   {
      uint8_t data = ( (uint8_t*)pData )[transmitIdx++];
      if ( ( data == FRAME_STARTBYTE ) || ( data == FRAME_STOPBYTE ) || ( data == ESCAPE_BYTE ) )
      {
         usart->write( ESCAPE_BYTE );
         data &= 0x7F;
      }
      usart->write( data );
   }
   usart->write( FRAME_STOPBYTE );
   usart->waitUntilTransferCompleted();

   // check if transmission was successful
   // 1. transmitter will be disabled if collision is detected
   // 2. compare the receiveBuffer against the data that should be sent
   // return 0 to indicate that transmission has been failed
   IStream::Status status = IStream::SUCCESS;
   CriticalSection doNotInterrupt;
   if ( !isTransmitterEnabled() || memcmp( receiveBuffer, pData, length ) )
   {
      status = IStream::ABORTED;
   }
   disableTransmitter();
   transmitBuffer = NULL;
   TRACE_PORT_CLEAR( TX_MSG );
   return status;
}

