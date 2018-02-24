/*
 * BooterLoaderHw.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "BooterLoaderHw.h"
#include <BooterFw.h>
#include <Security/Checksum.h>


void BooterLoaderHw::copyBooter()
{
   uint8_t pageBuffer[Flash::getPageSize()];
   Flash::address_t src = reinterpret_cast<Flash::address_t>( BooterFw );
   Flash::address_t dest = BOOT_SECTION_START;

   uint16_t i;

   for ( i = 0; i < sizeof( BooterFw ); i += Flash::getPageSize() )
   {
      PORTR.OUTSET = Pin1;
      Flash::read( src + i, pageBuffer, Flash::getPageSize() );
      Flash::writeBootSection( dest + i, pageBuffer, Flash::getPageSize() );
      PORTR.OUTCLR = Pin1;
   }

   // verify
   src = reinterpret_cast<Flash::address_t>( BooterFw );
   dest = BOOT_SECTION_START;
   for ( i = 0; i < sizeof( BooterFw ); i++ )
   {
      if ( Flash::read( src ) != Flash::read( dest ) )
      {
         while ( 1 )
         {
         }
      }
   }

   Flash::eraseApplication();
}
