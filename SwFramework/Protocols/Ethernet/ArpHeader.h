/*
 * ArpHeader.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_Ethernet_ArpHeader_H
#define SwFramework_Protocols_Ethernet_ArpHeader_H

#include "Ethernet.h"
#include "EthernetHeader.h"
#include "IP.h"
#include "MAC.h"

class ArpHeader : public EthernetHeader
{
   public:

      enum Opcodes {
         REQUEST = 1,
         REPLY = 2
      };

      enum HwTypes {
         HWTYPE_ETH = 1
      };

      ////    Operations    ////

      inline uint16_t getHwType() const
      {
         return changeEndianness( hwType );
      }

      inline uint16_t getOpcode() const
      {
         return changeEndianness( opcode );
      }

      inline uint16_t getProtocol() const
      {
         return changeEndianness( protocol );
      }

      inline void setHwType( uint16_t p_hwType )
      {
         hwType = changeEndianness( p_hwType );
      }

      inline void setOpcode( uint16_t p_opcode )
      {
         opcode = changeEndianness( p_opcode );
      }

      inline void setProtocol( uint16_t p_protocol )
      {
         protocol = changeEndianness( p_protocol );
      }

      ////    Additional operations    ////

      inline uint8_t getHwLength() const
      {
         return hwLength;
      }

      inline void setHwLength( uint8_t p_hwLength )
      {
         hwLength = p_hwLength;
      }

      inline uint8_t getProtocolLength() const
      {
         return protocolLength;
      }

      inline void setProtocolLength( uint8_t p_protocolLength )
      {
         protocolLength = p_protocolLength;
      }

      inline MAC* getSourceHwAddress() const
      {
         return (MAC*) &sourceHwAddress;
      }

      inline IP* getSourceIpAddress() const
      {
         return (IP*) &sourceIpAddress;
      }

      inline MAC* getDestinationHwAddress() const
      {
         return (MAC*) &destinationHwAddress;
      }

      inline IP* getDestinationIpAddress() const
      {
         return (IP*) &destinationIpAddress;
      }

      ////    Attributes    ////

      uint16_t hwType;

      uint16_t protocol;

      uint8_t hwLength;

      uint8_t protocolLength;

      uint16_t opcode;

      MAC sourceHwAddress;

      IP sourceIpAddress;

      MAC destinationHwAddress;

      IP destinationIpAddress;
};

#endif
