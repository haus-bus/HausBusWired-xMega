/*
 * Usart.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "Usart.h"

uint16_t Usart::read( void* pData, uint16_t length )
{
   uint8_t* data = static_cast<uint8_t*>( pData );
   uint16_t len = length;

   while ( len-- )
   {
      if ( !read( *data++ ) )
      {
         return ( length - len );
      }
   }
   return length;
}

uint16_t Usart::write( void* pData, uint16_t length )
{
   uint8_t* data = static_cast<uint8_t*>( pData );
   uint16_t len = length;

   while ( len-- )
   {
      if ( !write( *data++ ) )
      {
         return ( length - len );
      }
   }

   return length;
}
