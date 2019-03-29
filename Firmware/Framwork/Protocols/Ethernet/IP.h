/*
 * IP.h
 *
 *  Created on: 06.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_Ethernet_IP_H
#define SwFramework_Protocols_Ethernet_IP_H

#include "Ethernet.h"

#define IP_TO_UINT32( a, b, c, d ) ( ( (uint32_t) ( d ) << 24 ) + ( (uint32_t) ( c ) << 16 ) + ( (uint32_t) ( b ) << 8 ) + a )

class IP
{
   ////    Constructors and destructors    ////

   public:

      inline void set( uint8_t a = 0, uint8_t b = 0, uint8_t c = 0, uint8_t d = 0 )
      {
         setAddress(
            ( (uint32_t) ( d ) << 24 ) + ( (uint32_t) ( c ) << 16 ) + ( (uint32_t) ( b ) << 8 )
            + a );
      }

      ////    Operations    ////

      inline void clear()
      {
         address = 0;
      }

      inline static IP& broadcast()
      {
         static IP ip = { .address = 0xFFFFFFFF };
         return ip;
      }

      inline IP getMaskedIp() const;

      inline bool isBroadcast();

      inline bool isLocalBroadcast();

      inline bool isForMe() const;

      inline bool isInLocalNetwork() const;

      inline bool isValid() const
      {
         return address != 0;
      }

      inline bool operator==( const IP& _ip )
      {
         return this->address == _ip.address;
      }

      inline bool operator==( const uint32_t& _ip )
      {
         return this->address == _ip;
      }

      inline void setBroadcast()
      {
         address = 0xFFFFFFFF;
      }

      inline void setLocalBroadcast()
      {
         address = local.address | 0xFF000000;
      }

      ////    Additional operations    ////

      inline uint32_t getAddress() const
      {
         return address;
      }

      inline void setAddress( uint32_t p_address )
      {
         address = p_address;
      }

      inline static IP* getLocal()
      {
         return (IP*) &local;
      }

      inline static IP* getNetmask()
      {
         return (IP*) &netmask;
      }

      inline static IP* getRouter()
      {
         return (IP*) &router;
      }

      ////    Attributes    ////

      uint32_t address;

      static IP local;

      static IP netmask;

      static IP router;

      static IP defaultIp;

      static IP defaultNetmask;

      static IP defaultRouterIp;
};


inline IP IP::getMaskedIp() const
{
   IP maskedIP;
   maskedIP.setAddress( address & netmask.address );
   return maskedIP;
}

inline bool IP::isBroadcast()
{
   return address == 0xFFFFFFFF;
}

inline bool IP::isLocalBroadcast()
{
   return ( address & 0xFF000000 ) == 0xFF000000;
}

inline bool IP::isForMe() const
{
   return ( local == address );
}

inline bool IP::isInLocalNetwork() const
{
   return getMaskedIp() == local.getMaskedIp();
}

#endif
