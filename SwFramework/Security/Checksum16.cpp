/*
 * Checksum16.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Checksum16.h"

uint16_t Checksum16::get( void* pData, uint16_t length, uint32_t checksum )
{
   uint8_t* data = static_cast<uint8_t*>( pData );
   uint8_t dataH;
   uint8_t dataL;
   uint16_t help16;

   while ( length > 1 )
   {
      // read the word big Indian
      dataH = *data++;
      dataL = *data++;
      help16 = ( dataH << 8 ) + dataL;

      checksum += help16;
      length -= 2;
   }

   // handle odd length
   if ( length > 0 )
   {
      help16 = ( *data << 8 );
      checksum += help16;
   }

   // Komplementbildung (addiert Long INT_H Byte mit Long INT L Byte)
   checksum = ( ( checksum & 0x0000FFFF ) + ( ( checksum & 0xFFFF0000 ) >> 16 ) );
   checksum = ( ( checksum & 0x0000FFFF ) + ( ( checksum & 0xFFFF0000 ) >> 16 ) );

   return ~( checksum & 0x0000FFFF );
}
