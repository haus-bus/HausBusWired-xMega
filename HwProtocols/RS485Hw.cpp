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
   TX_MSG = Pin4,
	
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
      receiveBufferSize = 0;
   }
   else if ( data == FRAME_STOPBYTE )
   {
      TRACE_PORT_CLEAR( RX_MSG );
      // message completed
      if ( transmitBuffer )
      {
         // sending operation was in progress
         disableTransmitter();
         transmitBuffer = NULL;
         receiveBufferSize = 0;
      }
      else if ( user )
      {
         Stream::TransferDescriptor td;
         td.setBytesTransferred( receiveBufferSize );
         td.setPData( receiveBuffer );
         evData( user, &td ).send();
      }
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
            usart->disableTransmitter();
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
   bool hasError = usart->init<56000>();
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
   CriticalSection doNotInterrupt;
   this->user = user;
   /*
      if ( receiveBuffer && (receiveBuffer[0] == FRAME_STARTBYTE ) && (receiveBuffer[receiveBufferSize] == FRAME_STOPBYTE ) )
      {
      length = receiveBufferSize;
      memcpy( pData, receiveBuffer, length );
      receiveBufferSize = 0;
      DEBUG_H1( FSTR( "Msg recieved" ) );
      DEBUG_M2( FSTR( "received bytes: 0x" ), length );
      return Stream::SUCCESS;
      }
      else if ( receiveBufferSize )
      {
      DEBUG_H2( FSTR( "received bytes: 0x" ), receiveBufferSize );
      }
    */
   return Stream::NO_DATA;
}

Stream::Status RS485Hw::write( void* pData, uint16_t length, EventDrivenUnit* user )
{
   TRACE_PORT_SET( TX_MSG );
   DEBUG_H3( FSTR( "sending 0x" ), length, FSTR( " Bytes" ) );
   usart->enableTransmitter();
   enableTransmitter();

   transmitBuffer = (uint8_t*) pData;
   uint16_t transmitIdx = 0;
   bool success = usart->write( FRAME_STARTBYTE );

   while ( success && ( transmitIdx < length ) )
   {
      uint8_t data = ( (uint8_t*)pData )[transmitIdx++];
      if ( ( data == FRAME_STARTBYTE ) || ( data == FRAME_STOPBYTE ) || ( data == ESCAPE_BYTE ) )
      {
         success &= usart->write( ESCAPE_BYTE );
         data &= 0x7F;
      }
      if ( success )
      {
         success &= usart->write( data );
      }
   }
   if ( success )
   {
      success &= usart->write( FRAME_STOPBYTE );
   }
   usart->waitUntilTransferCompleted();
   disableTransmitter();

   // check if transmission was successful
   // 1. usart transmitter will be disabled if collision is detected
   // 2. compare the receiveBuffer against the data that should be sent
   // return 0 to indicate that transmission has been failed
   Stream::Status status = Stream::SUCCESS;
   CriticalSection doNotInterrupt;
   if ( !usart->isTransmitterEnabled() || memcmp( receiveBuffer, pData, length ) )
   {
      status = Stream::ABORTED;
   }
   TRACE_PORT_CLEAR( TX_MSG );
   return status;
}

