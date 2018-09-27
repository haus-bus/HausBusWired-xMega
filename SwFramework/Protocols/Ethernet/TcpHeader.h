/*
 * TcpHeader.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_Ethernet_TcpHeader_H
#define SwFramework_Protocols_Ethernet_TcpHeader_H

#include "Ethernet.h"
#include "IpHeader.h"

class TcpHeader : public IpHeader
{
   public:

      enum Flags {
         F_FIN = 0x01,
         F_SYN = 0x02,
         F_RST = 0x04,
         F_PSH = 0x08,
         F_ACK = 0x10,
         F_URG = 0x20,
         F_CTL = 0x3F
      };

      enum Options {
         OPT_END,
         OPT_NOOP,
         OPT_MSS,
         OPT_MSS_LEN = 4
      };

      ////    Operations    ////

      inline void changeToAckPacket();

      inline void changeToFinAckPacket();

      void changeToResetAckPacket();

      void changeToResetPacket();

      inline uint32_t getAckNumber() const
      {
         return changeEndianness( ackNumber );
      }

      inline uint16_t getChecksum() const
      {
         return changeEndianness( checksum );
      }

      // returns dataOffset in bytes after TcpHeader
      uint8_t* getData() const;

      // returns dataOffset in bytes after TcpHeader
      uint16_t getDataLength() const;

      // returns dataOffset in bytes after TcpHeader
      uint16_t getDataOffset() const;

      inline uint16_t getDestinationPort() const
      {
         return changeEndianness( destinationPort );
      }

      // returns dataOffset in bytes after TcpHeader
      uint8_t* getOptions() const;

      inline uint32_t getSeqNumber() const
      {
         return changeEndianness( seqNumber );
      }

      inline uint16_t getSourcePort() const
      {
         return changeEndianness( sourcePort );
      }

      inline uint16_t getUrgentPointer() const
      {
         return changeEndianness( urgentPointer );
      }

      inline uint16_t getWindow() const
      {
         return changeEndianness( window );
      }

      inline bool isACK() const;

      inline bool isChecksumCorrect();

      inline bool isDestinationPort( uint16_t port ) const;

      inline bool isFIN() const;

      inline bool isOnlySYN() const;

      inline bool isRST() const;

      inline bool isSYN() const;

      inline bool isSYN_ACK() const;

      inline bool isURG() const;

      void prepareSynAck( uint16_t maxSegmentSize );

      inline void setAckNumber( const uint32_t& p_ackNumber )
      {
         ackNumber = changeEndianness( p_ackNumber );
      }

      inline void setChecksum( uint16_t p_checksum )
      {
         checksum = changeEndianness( p_checksum );
      }

      void setChecksum();

      void setDataOffset( uint8_t p_dataOffset );

      inline void setDestinationPort( uint16_t p_destinationPort )
      {
         destinationPort = changeEndianness( p_destinationPort );
      }

      inline void setSeqNumber( const uint32_t& p_seqNumber )
      {
         seqNumber = changeEndianness( p_seqNumber );
      }

      inline void setSourcePort( uint16_t p_sourcePort )
      {
         sourcePort = changeEndianness( p_sourcePort );
      }

      inline void setUrgentPointer( uint16_t p_urgentPointer )
      {
         urgentPointer = changeEndianness( p_urgentPointer );
      }

      inline void setWindow( uint16_t p_window )
      {
         window = changeEndianness( p_window );
      }

      ////    Additional operations    ////

      inline uint8_t getFlags() const
      {
         return flags;
      }

      inline void setFlags( uint8_t p_flags )
      {
         flags = p_flags;
      }

      ////    Attributes    ////

      uint16_t sourcePort;

      uint16_t destinationPort;

      uint32_t seqNumber;

      uint32_t ackNumber;

      uint8_t dataOffset;

      uint8_t flags;

      uint16_t window;

      uint16_t checksum;

      uint16_t urgentPointer;
};

inline void TcpHeader::changeToAckPacket()
{
   flags = F_ACK;
   setDataOffset( 0 );
}

inline void TcpHeader::changeToFinAckPacket()
{
   flags = F_FIN | F_ACK;
   setDataOffset( 0 );
}

inline bool TcpHeader::isACK() const
{
   return ( flags & F_ACK ) == F_ACK;
}

inline bool TcpHeader::isChecksumCorrect()
{
   // calculate data length and add Pseudoheaderlength 2xIP = 8
   uint16_t len = getPacketSize() - ( sizeof( IpHeader ) - sizeof( EthernetHeader ) ) + 8;
   uint16_t startValue = len - 8 + IpHeader::TCP_PROTOCOL;
   return ( Checksum16::get( &sourceAddress, len, startValue ) == 0 );
}

inline bool TcpHeader::isDestinationPort( uint16_t port ) const
{
   return ( getDestinationPort() == port );
}

inline bool TcpHeader::isFIN() const
{
   return ( flags & F_FIN ) == F_FIN;
}

inline bool TcpHeader::isOnlySYN() const
{
   return ( ( flags & F_CTL ) == F_SYN );
}

inline bool TcpHeader::isRST() const
{
   return ( flags & F_RST ) == F_RST;
}

inline bool TcpHeader::isSYN() const
{
   return ( flags & F_SYN ) == F_SYN;
}

inline bool TcpHeader::isSYN_ACK() const
{
   return ( flags & F_CTL ) == ( F_SYN | F_ACK );
}

inline bool TcpHeader::isURG() const
{
   return ( flags & F_URG ) == F_URG;
}

#endif
