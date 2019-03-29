/*
 * IpHeader.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_Ethernet_IpHeader_H
#define SwFramework_Protocols_Ethernet_IpHeader_H

#include "Ethernet.h"
#include "Security/Checksum16.h"
#include "EthernetHeader.h"
#include "IP.h"

class IpHeader : public EthernetHeader
{
   public:

      static const uint8_t ICMP_PROTOCOL = 0x01;
      static const uint8_t TCP_PROTOCOL = 0x06;
      static const uint8_t UDP_PROTOCOL = 0x11;

      static const uint16_t DO_NOT_FRAGMENT = 0x4000;
      static const uint16_t MORE_FRAGMENTS = 0x8000;

      static const uint8_t MIN_PACKET_SIZE = 60;

      ////    Operations    ////

      inline uint16_t getChecksum() const
      {
         return changeEndianness( checksum );
      }

      inline uint16_t getFragmentOffset() const
      {
         return changeEndianness( fragmentOffset );
      }

      inline uint16_t getId() const
      {
         return changeEndianness( id );
      }

      inline uint16_t getPacketSize() const
      {
         return changeEndianness( packetSize );
      }

      inline bool isChecksumCorrect();

      inline bool isFragmented();

      inline bool isPacketSizeValid( uint16_t dataLength )
      {
         uint16_t dataSize = getPacketSize() + sizeof( EthernetHeader );
         return ( dataSize <= dataLength );
      }

      inline bool isProtocollIcmp();

      inline bool isProtocollTcp();

      inline bool isProtocollUdp();

      inline bool isValid();

      inline void setChecksum( uint16_t p_checksum )
      {
         checksum = changeEndianness( p_checksum );
      }

      void setChecksum();

      inline void setFragmentOffset( uint16_t p_fragmentOffset )
      {
         fragmentOffset = changeEndianness( p_fragmentOffset );
      }

      inline void setId( uint16_t p_id )
      {
         id = changeEndianness( p_id );
      }

      inline void setPacketSize( uint16_t p_packetSize )
      {
         packetSize = changeEndianness( p_packetSize );
      }

      ////    Additional operations    ////

      inline uint8_t getLength() const
      {
         return length;
      }

      inline void setLength( uint8_t p_length )
      {
         length = p_length;
      }

      inline uint8_t getVersion() const
      {
         return version;
      }

      inline void setVersion( uint8_t p_version )
      {
         version = p_version;
      }

      inline uint8_t getTypeOfService() const
      {
         return typeOfService;
      }

      inline void setTypeOfService( uint8_t p_typeOfService )
      {
         typeOfService = p_typeOfService;
      }

      inline uint8_t getTimeToLive() const
      {
         return timeToLive;
      }

      inline void setTimeToLive( uint8_t p_timeToLive )
      {
         timeToLive = p_timeToLive;
      }

      inline uint8_t getProtocol() const
      {
         return protocol;
      }

      inline void setProtocol( uint8_t p_protocol )
      {
         protocol = p_protocol;
      }

      inline IP* getSourceAddress() const
      {
         return (IP*) &sourceAddress;
      }

      inline IP* getDestinationAddress() const
      {
         return (IP*) &destinationAddress;
      }

      ////    Attributes    ////

      uint8_t length : 4;

      uint8_t version : 4;

      uint8_t typeOfService;

      uint16_t packetSize;

      uint16_t id;

      uint16_t fragmentOffset;

      uint8_t timeToLive;

      uint8_t protocol;

      uint16_t checksum;

      IP sourceAddress;

      IP destinationAddress;
};

inline bool IpHeader::isChecksumCorrect()
{
   return Checksum16::get( ( (EthernetHeader*)this ) + 1, getLength() << 2 ) == 0;
}

inline bool IpHeader::isFragmented()
{
   return ( fragmentOffset & 0xFF3F ) != 0;
}

inline bool IpHeader::isProtocollIcmp()
{
   return ( protocol == ICMP_PROTOCOL );
}

inline bool IpHeader::isProtocollTcp()
{
   return ( protocol == TCP_PROTOCOL );
}

inline bool IpHeader::isProtocollUdp()
{
   return ( protocol == UDP_PROTOCOL );
}

inline bool IpHeader::isValid()
{
   return ( version == 4 ) && ( length == 5 );
}

#endif
