/*
 * Flash.cpp
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#include "Flash.h"

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
   if ( ( address & ( getPageSize() - 1 ) ) || ( ( address + length ) > getMaxApplicationSize() ) )
   {
      return 0;
   }

   loadFlashPageBuffer( reinterpret_cast<uint8_t*>( pData ) );
   eraseWriteApplicationPage( address );
   enableApplicationSection();

   return length;
}
