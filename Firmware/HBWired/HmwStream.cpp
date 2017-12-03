/*
 * HmWStream.cpp
 *
 * Created: 18.11.2017 17:32:07
 * Author: viktor.pankraz
 */


#include "HmwStream.h"

HmwStreamHw* HmwStream::hardware( NULL );

uint8_t HmwStream::senderNum( 0 );

uint8_t HmwStream::receiverNum( 0 );

Timestamp HmwStream::lastReceivedTime( 0 );

HmwStream::CommunicationStatus HmwStream::statusSending;

HmwStream::CommunicationStatus HmwStream::statusReceiving;

HmwMessageBase inMessage;

const uint8_t HmwStream::debugLevel( DEBUG_LEVEL_LOW );

#include "DigitalOutput.h"
Stream::Status HmwStream::sendMessage( HmwMessageBase* msg )
{
   if ( !hardware )
   {
      return Stream::LOCKED;
   }

   // wait for bus to be idle
   while ( !isIdle() && !msg->isACK() )
   {
   }

   uint8_t data;
   Stream::Status status = Stream::SUCCESS;

   statusSending.msg = msg;
   statusSending.dataIdx = 0;
   statusSending.transmitting = false;

   if ( msg->isInfo() )
   {
      msg->setSenderNum( senderNum );
      if ( msg->isBroadcast() )
      {
         msg->setSyncBit();
      }
   }

   hardware->enableTranceiver( true );
   while ( HmwStream::getNextByteToSend( data ) )
   {
      if ( !hardware->serial->write( data ) )
      {
         status = Stream::ABORTED;
      }
   }
   hardware->serial->waitUntilTransferCompleted();
   hardware->enableTranceiver( false );

   if ( status == Stream::SUCCESS )
   {
      senderNum++;
   }
   lastReceivedTime = Timestamp();

   return status;
}

HmwMessageBase* HmwStream::pollMessageReceived()
{
   if ( hardware && hardware->serial->isReceiveCompleted() )
   {
      lastReceivedTime = Timestamp();
      uint8_t data;
      if ( hardware->serial->read( data ) )
      {
         if ( HmwStream::nextByteReceived( data ) )
         {
            return statusReceiving.msg;
         }
      }
   }
   return NULL;
}

bool HmwStream::nextByteReceived( uint8_t data )
{
   if ( statusReceiving.msg == NULL )
   {
      statusReceiving.msg = &inMessage;
   }

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
         statusReceiving.msg->init();
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
         statusReceiving.msg->setRawByte( statusReceiving.dataIdx, data );

         if ( statusReceiving.dataIdx == HmwMessageBase::ADDRESS_SIZE )
         {
            // controlByte was read
            if ( !statusReceiving.msg->hasSenderAddress() )
            {
               statusReceiving.dataIdx += HmwMessageBase::ADDRESS_SIZE;
            }
         }
         else if ( statusReceiving.dataIdx == HmwMessageBase::HEADER_SIZE )
         {
            // frameDataLength was read
            if ( statusReceiving.msg->getFrameDataLength() > HmwMessageBase::MAX_FRAME_LENGTH )               // check for max farme length
            {
               statusReceiving.transmitting = false;
               DEBUG_M1( FSTR( "E: MsgTooLong" ) );
            }
         }
         else if ( statusReceiving.dataIdx == ( HmwMessageBase::HEADER_SIZE + statusReceiving.msg->getFrameDataLength() ) )
         {
            // data complete
            if ( statusReceiving.crc16checksum == 0 )              //
            {
               // correct endianess and length
               statusReceiving.msg->convertToLittleEndian();
               statusReceiving.msg->setFrameDataLength( statusReceiving.msg->getFrameDataLength() - sizeof( statusReceiving.crc16checksum ) );
               statusReceiving.transmitting = false;
               statusReceiving.msg->setValid( true );

               // if sync bit is set, reset the static senderNum
               if ( statusReceiving.msg->isSync() )
               {
                  senderNum = 0;
                  receiverNum = statusReceiving.msg->getSenderNum();
               }
               return true;
            }
            else
            {
               DEBUG_M1( FSTR( "E: CRC" ) );
            }
         }
         statusReceiving.dataIdx++;
      }
   }
   return false;
}

bool HmwStream::getNextByteToSend( uint8_t& data )
{
   if ( statusSending.msg == NULL )
   {
      // ToDo look into the queue and send the next message
      return false;
   }

   if ( ( statusSending.dataIdx == 0 ) && !statusSending.transmitting )
   {
      statusSending.transmitting = true;
      statusSending.crc16checksum = 0xFFFF;
      statusSending.msg->convertToBigEndian();
      data = HmwMessageBase::FRAME_STARTBYTE;
      HmwMessageBase::crc16Shift( data, statusSending.crc16checksum );
      DEBUG_M3( FSTR( "T: " ), data, '|' );
      return true;
   }
   else if ( statusSending.transmitting )
   {
      data = statusSending.msg->getRawByte( statusSending.dataIdx );

      if ( statusSending.dataIdx == HmwMessageBase::ADDRESS_SIZE )
      {
         // controlByte was sent
         if ( !statusSending.msg->hasSenderAddress() )
         {
            statusSending.dataIdx += HmwMessageBase::ADDRESS_SIZE;
         }
      }
      else if ( statusSending.dataIdx == HmwMessageBase::HEADER_SIZE )
      {
         // modify the frameDataLength by the size of checksum
         data += sizeof( statusSending.crc16checksum );
      }
      else if ( statusSending.dataIdx > ( HmwMessageBase::HEADER_SIZE + statusSending.msg->getFrameDataLength() + sizeof( statusSending.crc16checksum ) ) )
      {
         // sending complete, no data available
         return false;
      }

      if ( !statusSending.pendingEscape )
      {
         DEBUG_L2( data, '|' );
         HmwMessageBase::crc16Shift( data, statusSending.crc16checksum );
         if ( statusSending.dataIdx == ( HmwMessageBase::HEADER_SIZE + statusSending.msg->getFrameDataLength() ) )
         {
            // last data byte will be sent, calculate crc
            HmwMessageBase::crc16Shift( 0, statusSending.crc16checksum );
            HmwMessageBase::crc16Shift( 0, statusSending.crc16checksum );
            statusSending.msg->setRawByte( statusSending.dataIdx + 1, HBYTE( statusSending.crc16checksum ) );
            statusSending.msg->setRawByte( statusSending.dataIdx + 2, LBYTE( statusSending.crc16checksum ) );
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

   return false;
}