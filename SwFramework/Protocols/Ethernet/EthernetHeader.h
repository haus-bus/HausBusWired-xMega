/*
 * EthernetHeader.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_Ethernet_EthernetHeader_H
#define SwFramework_Protocols_Ethernet_EthernetHeader_H

#include "Ethernet.h"
#include "MAC.h"

class EthernetHeader
{
   public:

      static const uint16_t ARP = 0x0806;
      static const uint16_t IP_DATAGRAMM = 0x0800;
      static const uint16_t IP6_DATAGRAMM = 0x86DD;

      ////    Operations    ////

      inline uint16_t getPacketType() const
      {
         return changeEndianness( packetType );
      }

      inline bool isArp() const;

      inline bool isIpDatagramm() const;

      inline void setPacketType( uint16_t p_packetType )
      {
         packetType = changeEndianness( p_packetType );
      }

      ////    Additional operations    ////

      inline MAC* getPacketDestination() const
      {
         return (MAC*) &packetDestination;
      }

      inline MAC* getPacketSource() const
      {
         return (MAC*) &packetSource;
      }

      ////    Attributes    ////

      MAC packetDestination;

      MAC packetSource;

      uint16_t packetType;
};

inline bool EthernetHeader::isArp() const
{
   return ( getPacketType() == ARP );
}

inline bool EthernetHeader::isIpDatagramm() const
{
   return ( getPacketType() == IP_DATAGRAMM );
}

#endif
