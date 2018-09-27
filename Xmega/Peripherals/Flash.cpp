/*
 * Flash.cpp
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#include "Flash.h"
#include <string.h>

uint16_t Flash::read( Flash::address_t address, void* pData, uint16_t length )
{
   if ( ( address + length ) > FLASHEND )
   {
      return 0;
   }

   uint8_t* destination = static_cast<uint8_t*>( pData );

   uint16_t len = length;
   while ( len-- )
   {
      *destination++ = read( address );
   }

   return length;
}

uint16_t Flash::write( Flash::address_t address, void* pData, uint16_t length )
{
   if ( ( address + length ) > getMaxApplicationSize() )
   {
      return 0;
   }
   uint8_t buffer[APP_SECTION_PAGE_SIZE];
   uint16_t offset = address & ( sizeof( buffer ) - 1 );
   address -= offset;

   // fill the buffer with the current data and update the new part
   Flash::read( address, buffer, sizeof( buffer ) );
   memcpy( &buffer[offset], pData, length );
   loadFlashPageBuffer( buffer );
   eraseWriteApplicationPage( address );
   enableApplicationSection();

   return length;
}
