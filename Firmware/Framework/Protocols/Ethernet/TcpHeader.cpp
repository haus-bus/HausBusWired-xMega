/*
 * TcpHeader.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "TcpHeader.h"

void TcpHeader::changeToResetAckPacket()
{
   flags = F_RST | F_ACK;
   setDataOffset( 0 );
}

void TcpHeader::changeToResetPacket()
{
   flags = F_RST | F_ACK;
   setDataOffset( 0 );

   // Flip the seqNumber and ackNumber fields
   // We also have to increase the sequence number we are
   // acknowledging. If the least significant byte overflowed, we need
   // to propagate the carry to the other bytes as well.
   uint32_t c = getSeqNumber() + 1;
   seqNumber = ackNumber;
   setAckNumber( c );

   // Swap port numbers.
   uint16_t tmp16 = sourcePort;
   sourcePort = destinationPort;
   destinationPort = tmp16;

   // Swap IP addresses.
   destinationAddress = sourceAddress;
   sourceAddress = IP::local;
}

uint8_t* TcpHeader::getData() const
{
   if ( isURG() )
   {
      return ( (uint8_t*)( this + 1 ) ) + getUrgentPointer();
   }
   return ( (uint8_t*)( this + 1 ) ) + getDataOffset();
}

uint16_t TcpHeader::getDataLength() const
{
   uint16_t length = getPacketSize();
   if ( length > ( sizeof( TcpHeader ) - sizeof( EthernetHeader ) ) )
   {
      uint16_t offset = sizeof( TcpHeader ) - sizeof( EthernetHeader );
      offset += ( isURG() ? getUrgentPointer() : getDataOffset() );
      if ( length > offset )
      {
         return length - offset;
      }
   }
   return 0;
}

uint16_t TcpHeader::getDataOffset() const
{
   if ( isURG() )
   {
      return getUrgentPointer();
   }
   return ( dataOffset >> 2 ) - ( sizeof( TcpHeader ) - sizeof( IpHeader ) );
}

uint8_t* TcpHeader::getOptions() const
{
   return (uint8_t*)( this + 1 );
}

void TcpHeader::prepareSynAck( uint16_t maxSegmentSize )
{
   flags = F_SYN | F_ACK;
   uint8_t* optdata = getOptions();
   optdata[0] = OPT_MSS;
   optdata[1] = OPT_MSS_LEN;
   optdata[2] = HBYTE( maxSegmentSize );
   optdata[3] = LBYTE( maxSegmentSize );
   setDataOffset( OPT_MSS_LEN );
}

void TcpHeader::setChecksum()
{
   checksum = 0;
   // calculate data length and add Pseudoheaderlength 2xIP = 8
   uint16_t len = getPacketSize() - ( sizeof( IpHeader ) - sizeof( EthernetHeader ) ) + 8;
   uint16_t startValue = len - 8 + IpHeader::TCP_PROTOCOL;
   setChecksum( Checksum16::get( &sourceAddress, len, startValue ) );
}

void TcpHeader::setDataOffset( uint8_t p_dataOffset )
{
   dataOffset = ( p_dataOffset + sizeof( TcpHeader ) - sizeof( IpHeader ) ) << 2;
}
