/*
 * Checksum.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Checksum.h"

uint8_t Checksum::get( const void* pData, uint16_t length, uint8_t checksum )
{
   const uint8_t* data = static_cast<const uint8_t*>( pData );

   while ( length-- )
   {
      checksum ^= *data++;
   }

   return checksum;
}
