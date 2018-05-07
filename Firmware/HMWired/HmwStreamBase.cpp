/*
 * HmWStreamBase.cpp
 *
 * Created: 13.12.2017 17:32:07
 * Author: viktor.pankraz
 */


#include "HmwStreamBase.h"
#include <DigitalOutput.h>

HmwStreamHw* HmwStreamBase::hardware( NULL );

uint8_t HmwStreamBase::senderNum( 0 );

uint8_t HmwStreamBase::receiverNum( 0 );

Timestamp HmwStreamBase::lastReceivedTime( 0 );

HmwStreamBase::CommunicationStatus HmwStreamBase::statusSending;

HmwStreamBase::CommunicationStatus HmwStreamBase::statusReceiving;

HmwMessageBase HmwStreamBase::inMessage;

const uint8_t HmwStreamBase::debugLevel( DEBUG_LEVEL_OFF );

#define RX_TRACE_PIN            Pin0
#define RX_ERROR_TRACE_PIN      Pin1


Stream::Status HmwStreamBase::sendMessage( HmwMessageBase& msg )
{
   if ( !hardware )
   {
      return Stream::LOCKED;
   }

   // wait for bus to be idle, only ACKs are sent immediately
   while ( !isIdle() && !msg.isACK() )
   {
   }

   uint8_t data;
   Stream::Status status = Stream::SUCCESS;

   statusSending.msg = &msg;
   statusSending.dataIdx = 0;
   statusSending.transmitting = false;

   if ( msg.isInfo() && !msg.getSendingTries() )
   {
      msg.setSenderNum( senderNum );
   }

   hardware->enableTranceiver( true );
   while ( getNextByteToSend( data ) )
   {
      if ( !hardware->serial->write( data ) )
      {
         status = Stream::ABORTED;
      }
   }
   hardware->serial->waitUntilTransferCompleted();
   hardware->enableTranceiver( false );

   if ( ( status == Stream::SUCCESS ) && msg.isInfo() )
   {
      senderNum++;
   }

   return status;
}

HmwMessageBase* HmwStreamBase::pollMessageReceived()
{
   if ( hardware && hardware->serial->isReceiveCompleted() )
   {
      uint8_t data;
      if ( hardware->serial->read( data ) )
      {
         return nextByteReceived( data );
      }
   }
   return NULL;
}

HmwMessageBase* HmwStreamBase::nextByteReceived( uint8_t data )
{
   TRACE_PORT_SET( RX_TRACE_PIN );
   lastReceivedTime = Timestamp();

   // Debug
   if ( data == HmwMessageBase::FRAME_STARTBYTE )
   {
      DEBUG_M1( FSTR( "R: " ) );
   }
   else
   {
      DEBUG_L1( FSTR( ":" ) );
   }
   DEBUG_L1( data );

   if ( data == HmwMessageBase::ESCAPE_BYTE )
   {
      if ( statusReceiving.pendingEscape )
      {
         // TODO: Wenn frameEscape gesetzt ist, dann sind das zwei Escapes hintereinander
         // Das ist eigentlich ein Fehler -> Fehlerbehandlung
      }
      statusReceiving.pendingEscape = true;
   }
   else
   {
      if ( data == HmwMessageBase::FRAME_STARTBYTE )       // start byte received?
      {
         statusReceiving.transmitting = true;
         statusReceiving.pendingEscape = false;
         statusReceiving.dataIdx = 0;
         statusReceiving.crc16checksum = 0xFFFF;
         inMessage.init();
         HmwMessageBase::crc16Shift( data, statusReceiving.crc16checksum );
      }
      else if ( statusReceiving.transmitting )       // start byte was found, frame should not be ignored
      {
         if ( statusReceiving.pendingEscape )
         {
            data |= 0x80;
            statusReceiving.pendingEscape = false;
         }
         HmwMessageBase::crc16Shift( data, statusReceiving.crc16checksum );
         inMessage.setRawByte( statusReceiving.dataIdx, data );

         if ( statusReceiving.dataIdx == HmwMessageBase::ADDRESS_SIZE )
         {
            // controlByte was read
            if ( !inMessage.hasSenderAddress() )
            {
               statusReceiving.dataIdx += HmwMessageBase::ADDRESS_SIZE;
            }
         }
         else if ( statusReceiving.dataIdx == HmwMessageBase::FRAME_HEADER_SIZE )
         {
            // frameDataLength was read
            if ( inMessage.getFrameDataLength() > HmwMessageBase::MAX_FRAME_LENGTH )               // check for max farme length
            {
               statusReceiving.transmitting = false;
               ERROR_1( FSTR( "MsgTooLong" ) );
               TRACE_PORT_TOGGLE( RX_ERROR_TRACE_PIN );
            }
         }
         else if ( statusReceiving.dataIdx == ( HmwMessageBase::FRAME_HEADER_SIZE + inMessage.getFrameDataLength() ) )
         {
            // data complete
            if ( statusReceiving.crc16checksum == 0 )              //
            {
               // correct endianess and length
               inMessage.convertToLittleEndian();
               inMessage.setFrameDataLength( inMessage.getFrameDataLength() - sizeof( statusReceiving.crc16checksum ) );
               statusReceiving.transmitting = false;
               inMessage.setValid( true );
               TRACE_PORT_CLEAR( RX_TRACE_PIN );
               return &inMessage;
            }
            else
            {
               TRACE_PORT_TOGGLE( RX_ERROR_TRACE_PIN );
               ERROR_1( FSTR( "CRC " ) );
               for ( uint8_t i = 0; i < statusReceiving.dataIdx; i++ )
               {
                  ERROR_DATA_2( inMessage.getRawByte( i ), ' ' )
               }
            }
         }
         statusReceiving.dataIdx++;
      }
   }
   TRACE_PORT_CLEAR( RX_TRACE_PIN );
   return NULL;
}

bool HmwStreamBase::getNextByteToSend( uint8_t& data )
{
   HmwMessageBase* msg = statusSending.msg;

   if ( msg == NULL )
   {
      return false;
   }

   if ( ( statusSending.dataIdx == 0 ) && !statusSending.transmitting )
   {
      statusSending.transmitting = true;
      statusSending.crc16checksum = 0xFFFF;
      msg->convertToBigEndian();
      data = msg->isShort() ? HmwMessageBase::FRAME_STARTBYTE_SHORT : HmwMessageBase::FRAME_STARTBYTE;
      HmwMessageBase::crc16Shift( data, statusSending.crc16checksum );
      DEBUG_M1( FSTR( "T: " ) );
      DEBUG_L2( data, '|' );
      return true;
   }
   else if ( statusSending.transmitting )
   {
      if ( msg->isShort() && statusSending.dataIdx == 0 )
      {
         // send zero after start byte and skip both addresses
         data = 0;
         statusSending.dataIdx = HmwMessageBase::ADDRESS_SIZE - 1;
      }
      else
      {
         data = msg->getRawByte( statusSending.dataIdx );
      }


      if ( statusSending.dataIdx == HmwMessageBase::ADDRESS_SIZE )
      {
         // controlByte was sent
         if ( !msg->hasSenderAddress() )
         {
            statusSending.dataIdx += HmwMessageBase::ADDRESS_SIZE;
         }
      }
      else if ( statusSending.dataIdx == HmwMessageBase::FRAME_HEADER_SIZE )
      {
         // modify the frameDataLength by the size of checksum
         data += sizeof( statusSending.crc16checksum );
      }
      else if ( statusSending.dataIdx > ( HmwMessageBase::FRAME_HEADER_SIZE + msg->getFrameDataLength() + sizeof( statusSending.crc16checksum ) ) )
      {
         // sending complete, no data available
         return false;
      }

      if ( !statusSending.pendingEscape )
      {
         DEBUG_L2( data, '|' );
         HmwMessageBase::crc16Shift( data, statusSending.crc16checksum );
         if ( statusSending.dataIdx == ( HmwMessageBase::FRAME_HEADER_SIZE + msg->getFrameDataLength() ) )
         {
            // last data byte will be sent, calculate crc
            HmwMessageBase::crc16Shift( 0, statusSending.crc16checksum );
            HmwMessageBase::crc16Shift( 0, statusSending.crc16checksum );
            msg->setRawByte( statusSending.dataIdx + 1, HBYTE( statusSending.crc16checksum ) );
            msg->setRawByte( statusSending.dataIdx + 2, LBYTE( statusSending.crc16checksum ) );
         }
         if ( ( data == HmwMessageBase::FRAME_STARTBYTE ) || ( data == HmwMessageBase::FRAME_STARTBYTE_SHORT ) || ( data == HmwMessageBase::ESCAPE_BYTE ) )
         {
            statusSending.pendingEscape = true;
            data = HmwMessageBase::ESCAPE_BYTE;
         }
         else
         {
            statusSending.dataIdx++;
         }
      }
      else
      {
         data &= 0x7F;
         statusSending.pendingEscape = false;
         statusSending.dataIdx++;
      }
      return true;
   }
   msg->convertToLittleEndian();
   return false;
}