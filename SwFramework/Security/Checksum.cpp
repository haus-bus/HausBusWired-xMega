/*
 * Checksum.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Security/Checksum.h"

uint8_t Checksum::get( void* pData, uint16_t length, uint8_t checksum )
{
   uint8_t* data = static_cast<uint8_t*>( pData );

   while ( length-- )
   {
      checksum ^= *data++;
   }

   return checksum;
}
