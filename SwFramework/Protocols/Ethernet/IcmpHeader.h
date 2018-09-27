/*
 * IcmpHeader.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_Ethernet_IcmpHeader_H
#define SwFramework_Protocols_Ethernet_IcmpHeader_H

#include "Ethernet.h"
#include "IpHeader.h"

class IcmpHeader : public IpHeader
{
   public:

      static const uint8_t ECHO_REPLY = 0;
      static const uint8_t ECHO = 8;

      ////    Operations    ////

      inline uint16_t getChecksum() const
      {
         return changeEndianness( checksum );
      }

      inline bool isEchoRequest() const;

      inline void setChecksum( uint16_t p_checksum )
      {
         checksum = changeEndianness( p_checksum );
      }

      ////    Additional operations    ////

      inline uint8_t getType() const
      {
         return type;
      }

      inline void setType( uint8_t p_type )
      {
         type = p_type;
      }

      inline uint8_t getICode() const
      {
         return iCode;
      }

      inline void setICode( uint8_t p_iCode )
      {
         iCode = p_iCode;
      }

      inline uint16_t getId() const
      {
         return id;
      }

      inline void setId( uint16_t p_id )
      {
         id = p_id;
      }

      inline uint8_t getSeqNumber( int i1 ) const
      {
         return seqNumber[i1];
      }

      inline void setSeqNumber( int i1, uint8_t p_seqNumber )
      {
         seqNumber[i1] = p_seqNumber;
      }

      ////    Attributes    ////

      uint8_t type;

      uint8_t iCode;

      uint16_t checksum;

      uint16_t id;

      uint8_t seqNumber[4];
};

inline bool IcmpHeader::isEchoRequest() const
{
   return ( type == ECHO );
}

#endif
