/*
 * Spi.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "Spi.h"

Spi& Spi::instance( uint8_t portNumber )
{
   switch ( portNumber )
   {
#ifdef SPIC
      case PortC:
         return *reinterpret_cast<Spi*>( &SPIC );
#endif

#ifdef SPID
      case PortD:
         return *reinterpret_cast<Spi*>( &SPID );
#endif

#ifdef SPIE
      case PortE:
         return *reinterpret_cast<Spi*> ( &SPIE );
#endif

#ifdef SPIF
      case PortF:
         return *reinterpret_cast<Spi*> ( &SPIF );
#endif
   }
   fatalErrorLoop();
   return *(Spi*) 0;
}

uint16_t Spi::read( void* pData, uint16_t length )
{
   uint8_t* data = static_cast<uint8_t*>( pData );

   uint16_t len = length;
   while ( length-- )
   {
      if ( read( *data++ ) )
      {
         return length - len;
      }
   }
   return length;
}

uint16_t Spi::write( const void* pData, uint16_t length )
{
   const uint8_t* data = static_cast<const uint8_t*>( pData );

   uint16_t len = length;
   while ( len-- )
   {
      uint8_t byte = *data++;
      if ( write( byte ) )
      {
         return length - len;
      }
   }

   return length;
}
