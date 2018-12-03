/*
 * RS485Hw.cpp
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */

#include "RS485Hw.h"
#include <EventPkg/EventPkg.h>

#define getId() FSTR( "RS485Hw::" )
const uint8_t RS485Hw::debugLevel( TRACE_PORT | DEBUG_LEVEL_LOW );

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

void RS485Hw::handleDataReceived()
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
      transmissionPending = true;
   }
   else if ( data == FRAME_STOPBYTE )
   {
      TRACE_PORT_CLEAR( RX_MSG );
      // message completed
      if ( transmitBuffer == NULL )
      {
         rxMsgComplete = true;
      }
      transmissionPending = false;
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
}

bool RS485Hw::init()
{
   bool hasError = usart->init<135, -2, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc>();
   return hasError;
}

Stream::Status RS485Hw::genericCommand( IoStream::Command command, void* buffer )
{
   if ( command == IoStream::INIT )
   {
      DEBUG_H1( FSTR( "init..." ) );
      IoStream::CommandINIT* cmd = (IoStream::CommandINIT*) buffer;

      receiveBuffer = new uint8_t[cmd->buffersize];

      if ( !receiveBuffer )
      {
         return Stream::Status::INVALID_BUFFER;
      }
      maxReceiveBufferSize = cmd->buffersize;

      DEBUG_M2( FSTR( "bufferSize: 0x" ), maxReceiveBufferSize );
      bool error = init();
      if ( error )
      {
         ERROR_1( FSTR( "hardware init failed!" ) )
         return Stream::Status::ABORTED;
      }
      usart->enableReceiveCompleteInterrupt();

      return Stream::Status::SUCCESS;
   }
   return Stream::Status::ABORTED;
}

Stream::Status RS485Hw::read( void* pData, uint16_t length, EventDrivenUnit* user )
{
   Stream::TransferDescriptor td;
   {
      CriticalSection doNotInterrupt;
      if ( rxBufferOverflow )
      {
         rxBufferOverflow = false;
         return Stream::BUFFER_OVERFLOW;
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
      return Stream::SUCCESS;
   }
   return Stream::NO_DATA;
}

Stream::Status RS485Hw::write( void* pData, uint16_t length, EventDrivenUnit* user )
{
   TRACE_PORT_SET( TX_MSG );
   DEBUG_H3( FSTR( "sending 0x" ), length, FSTR( " Bytes" ) );

   if ( transmissionPending || rxMsgComplete )
   {
      // read first the incoming message because receiveBuffer will be overwritten with transmitting new message
      return Stream::LOCKED;
   }
   enableTransmitter();

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
   Stream::Status status = Stream::SUCCESS;
   CriticalSection doNotInterrupt;
   if ( !isTransmitterEnabled() || memcmp( receiveBuffer, pData, length ) )
   {
      status = Stream::ABORTED;
   }
   disableTransmitter();
   transmitBuffer = NULL;
   TRACE_PORT_CLEAR( TX_MSG );
   return status;
}

