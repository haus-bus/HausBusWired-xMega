/*
 * RS485Hw.cpp
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */

#include "RS485Hw.h"
#include <EventPkg/EventPkg.h>

#define getId() FSTR( "RS485Hw::" )
const uint8_t RS485Hw::debugLevel( DEBUG_LEVEL_LOW );

void RS485Hw::handleDataReceived()
{
   uint8_t data;
   usart->read( data );

   if ( data == FRAME_STARTBYTE )
   {
      receiveBufferSize = 0;
   }
   else if ( data == FRAME_STOPBYTE )
   {
      // message completed
      if ( transmitBuffer )
      {
         // sending operation was in progress
         disableTransmitter();
         transmitBuffer = NULL;
         receiveBufferSize = 0;
      }
   }
   else if ( data == ESCAPE_BYTE )
   {
      pendingEscape = true;
   }
   else
   {
      // restore data first
      if ( pendingEscape )
      {
         data |= 0x80;
         pendingEscape = false;
      }
      if ( transmitBuffer )
      {
         if ( transmitBuffer[receiveBufferSize] != data )
         {
            // notify collision
            disableTransmitter();
            usart->disableTransmitter();
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
}

bool RS485Hw::init()
{
   bool hasError = usart->init<56000>();
   if ( !hasError )
   {
      disableReceiver();
      disableTransmitter();
      disableRx.configOutput();
      enableTx.configOutput();
   }
   return hasError;
}

Stream::Status RS485Hw::init( void* buffer, uint16_t bufferSize )
{
   DEBUG_H1( FSTR( "init..." ) );
   receiveBuffer = (uint8_t*)buffer;
   maxReceiveBufferSize = bufferSize;

   if ( !receiveBuffer )
   {
      return Stream::Status::INVALID_BUFFER;
   }

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

uint16_t RS485Hw::read( void* pData, uint16_t length )
{
   CriticalSection doNotInterrupt;
   if ( receiveBuffer && (receiveBuffer[0] == FRAME_STARTBYTE ) && (receiveBuffer[receiveBufferSize] == FRAME_STOPBYTE ) )
   {  
      length = receiveBufferSize;
      memcpy( pData, receiveBuffer, length );
      receiveBufferSize = 0;
      DEBUG_H1( FSTR( "Msg recieved" ) );
      DEBUG_M2( FSTR( "received bytes: 0x" ), length );
      return length;
   }
   else if ( receiveBufferSize )
   {
      DEBUG_H2( FSTR( "received bytes: 0x" ), receiveBufferSize );  
   }
   return 0;
}

uint16_t RS485Hw::write( void* pData, uint16_t length )
{
   DEBUG_H3( FSTR( "sending 0x" ), length, FSTR( " Bytes" ) );
   usart->enableTransmitter();
   enableTransmitter();

   bool success = usart->write( FRAME_STARTBYTE );

   transmitBuffer = (uint8_t*) pData;
   uint16_t transmitIdx = 0;
   while ( success && ( transmitIdx < length ) )
   {
      uint8_t data = transmitBuffer[transmitIdx++];
      if ( ( data == FRAME_STARTBYTE ) || ( data == FRAME_STOPBYTE ) || ( data == ESCAPE_BYTE )  )
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
   if ( !usart->isTransmitterEnabled() || memcmp( receiveBuffer, pData, length ) )
   {
      ERROR_1( FSTR( "COLLISION!" ) );
      return 0;
   }
   return transmitIdx;
}

