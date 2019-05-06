/*
 * BooterLoader.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include <Security/ModuleId.h>
#include <Peripherals/ResetSystem.h>
#include <Peripherals/Flash.h>
#include <BooterFw.h>
#include <Release.h>

extern __attribute__( ( section( ".vectors" ) ) ) const uint16_t moduleIdCCU;
extern __attribute__( ( section( ".vectors" ) ) ) const ModuleId moduleId;

const uint16_t moduleIdCCU = ( Release::MAJOR << 8 ) | Release::MINOR;

const ModuleId moduleId =
{
   "$MOD$ HBWLoader",
   0,
   Release::MAJOR,
   Release::MINOR,
   DEVICE_ID,
   0
};

int main( int argc, char* argv[] )
{
   int status = 0;
   {
      uint8_t pageBuffer[Flash::getPageSize()];
      Flash::address_t src = reinterpret_cast<Flash::address_t>( BooterFw );
      Flash::address_t dest = BOOT_SECTION_START;

      uint16_t i;

      for ( i = 0; i < sizeof( BooterFw ); i += Flash::getPageSize() )
      {
         Flash::read( src + i, pageBuffer, Flash::getPageSize() );
         Flash::writeBootSection( dest + i, pageBuffer, Flash::getPageSize() );
      }

      Flash::eraseApplication();
      ResetSystem::reset();
      while ( 1 )
      {
      }
   }
   return status;
}

