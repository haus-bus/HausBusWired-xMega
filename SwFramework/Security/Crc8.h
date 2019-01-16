/*
 * Crc8.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Security_Crc8_H
#define SwFramework_Security_Crc8_H

#include "Security.h"
#include <DefaultTypes.h>

class Crc8
{
   ////    Operations    ////

   public:

      inline static void calculateNextCrc( uint8_t& crc, uint8_t data );

      static uint8_t hasError( void* pData, uint16_t length, uint8_t crc = 0 );
};

inline void Crc8::calculateNextCrc( uint8_t& crc, uint8_t data )
{
   data ^= crc;
   crc = 0;
   if ( data & 0x01 )
   {
      crc = 0x5E;
   }
   if ( data & 0x02 )
   {
      crc ^= 0xBC;
   }
   if ( data & 0x04 )
   {
      crc ^= 0x61;
   }
   if ( data & 0x08 )
   {
      crc ^= 0xC2;
   }
   if ( data & 0x10 )
   {
      crc ^= 0x9D;
   }
   if ( data & 0x20 )
   {
      crc ^= 0x23;
   }
   if ( data & 0x40 )
   {
      crc ^= 0x46;
   }
   if ( data & 0x80 )
   {
      crc ^= 0x8C;
   }
}

#endif
