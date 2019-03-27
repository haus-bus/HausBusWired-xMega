/*
 * Crc7.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Crc7.h"

uint8_t Crc7::get( void* pData, uint16_t length, uint8_t crc )
{
   uint8_t* data = reinterpret_cast<uint8_t*>( pData );
   while ( length-- )
   {
      uint8_t value = *data++;
      for ( uint8_t i = 0; i < 8; i++ )
      {
         crc <<= 1;
         if ( ( value & 0x80 ) ^ ( crc & 0x80 ) )
         {
            crc ^= 0x09;
         }
         value <<= 1;
      }
   }
   crc = ( crc << 1 ) | 1;
   return crc;
}
