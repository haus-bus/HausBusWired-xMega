/*
 * MAC.h
 *
 *  Created on: 06.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_Ethernet_MAC_H
#define SwFramework_Protocols_Ethernet_MAC_H

#include "Ethernet.h"

class MAC
{

   public:

      inline void set( uint8_t a = 0, uint8_t b = 0, uint8_t c = 0, uint8_t d = 0, uint8_t e = 0, uint8_t f = 0 )
      {
         address[0] = a;
         address[1] = b;
         address[2] = c;
         address[3] = d;
         address[4] = e;
         address[5] = f;
      }

      ////    Operations    ////

      inline bool isBroadcast();

      inline bool isLocal();

      inline bool isValid();

      inline void setBroadcast();

      ////    Additional operations    ////

      inline uint8_t getAddress( int i1 ) const
      {
         return address[i1];
      }

      inline void setAddress( int i1, uint8_t p_address )
      {
         address[i1] = p_address;
      }

      inline static MAC* getLocal()
      {
         return (MAC*) &local;
      }

      ////    Attributes    ////

      uint8_t address[6];

      static MAC local;
};

inline bool MAC::isBroadcast()
{
   uint8_t i = sizeof( address );
   while ( i-- )
   {
      if ( address[i] != 0xFF )
      {
         return false;
      }
   }
   return true;
}

inline bool MAC::isLocal()
{
   uint8_t i = sizeof( address );
   while ( i-- )
   {
      if ( address[i] != local.getAddress( i ) )
      {
         return false;
      }
   }
   return true;
}

inline bool MAC::isValid()
{
   uint8_t i = sizeof( address );
   while ( i-- )
   {
      if ( address[i] != 0 )
      {
         return true;
      }
   }
   return false;
}

inline void MAC::setBroadcast()
{
   uint8_t i = sizeof( address );
   while ( i-- )
   {
      address[i] = 0xFF;
   }
}

#endif
