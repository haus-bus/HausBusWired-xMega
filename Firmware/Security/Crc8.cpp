/*
 * Crc8.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Crc8.h"

uint8_t Crc8::hasError( void* pData, uint16_t length, uint8_t crc )
{
   uint8_t* data = static_cast<uint8_t*>( pData );

   for (; length; length-- )
   {
      calculateNextCrc( crc, *data++ );
   }

   return crc;
}
