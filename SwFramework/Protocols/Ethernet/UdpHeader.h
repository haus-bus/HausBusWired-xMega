/*
 * UdpHeader.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_Ethernet_UdpHeader_H
#define SwFramework_Protocols_Ethernet_UdpHeader_H

#include "Ethernet.h"
#include "IpHeader.h"

class UdpHeader : public IpHeader
{
   ////    Operations    ////

   public:

      inline uint16_t getChecksum() const
      {
         return changeEndianness( checksum );
      }

      inline uint16_t getDestinationPort() const
      {
         return changeEndianness( destinationPort );
      }

      inline uint16_t getLength() const
      {
         return changeEndianness( length );
      }

      inline uint16_t getSourcePort() const
      {
         return changeEndianness( sourcePort );
      }

      inline bool isChecksumCorrect();

      inline bool isDestinationPort( uint16_t port );

      inline void setChecksum( uint16_t p_checksum )
      {
         checksum = changeEndianness( p_checksum );
      }

      void setChecksum();

      inline void setDestinationPort( uint16_t p_destinationPort )
      {
         destinationPort = changeEndianness( p_destinationPort );
      }

      inline void setLength( uint16_t p_length )
      {
         length = changeEndianness( p_length );
      }

      inline void setSourcePort( uint16_t p_sourcePort )
      {
         sourcePort = changeEndianness( p_sourcePort );
      }

      ////    Attributes    ////

      uint16_t sourcePort;

      uint16_t destinationPort;

      uint16_t length;

      uint16_t checksum;
};

inline bool UdpHeader::isChecksumCorrect()
{
   // calculate data length and add Pseudoheaderlength 2xIP = 8
   uint16_t len = getPacketSize() - ( sizeof( IpHeader ) - sizeof( EthernetHeader ) ) + 8;
   uint16_t startValue = len - 8 + IpHeader::UDP_PROTOCOL;
   return ( Checksum16::get( &sourceAddress, len, startValue ) == 0 );
}

inline bool UdpHeader::isDestinationPort( uint16_t port )
{
   return ( getDestinationPort() == port );
}

#endif
