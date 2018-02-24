/*
 * Usart.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Usart.h"

Usart& Usart::instance( uint8_t portNumber, uint8_t channel )
{
   if ( channel == 0 )
   {
      switch ( portNumber )
      {
#ifdef USARTC0
         case PortC:
            return *reinterpret_cast<Usart*>( &USARTC0 );
#endif

#ifdef USARTD0
         case PortD:
            return *reinterpret_cast<Usart*>( &USARTD0 );
#endif

#ifdef USARTE0
         case PortE:
            return *reinterpret_cast<Usart*>( &USARTE0 );
#endif

#ifdef USARTF0
         case PortF:
            return *reinterpret_cast<Usart*>( &USARTF0 );
#endif
      }
   }
   else if ( channel == 1 )
   {
      switch ( portNumber )
      {
#ifdef USARTC1
         case PortC:
            return *reinterpret_cast<Usart*>( &USARTC1 );
#endif

#ifdef USARTD1
         case PortD:
            return *reinterpret_cast<Usart*>( &USARTD1 );
#endif

#ifdef USARTE1
         case PortE:
            return *reinterpret_cast<Usart*>( &USARTE1 );
#endif

#ifdef USARTF1
         case PortF:
            return *reinterpret_cast<Usart*> ( &USARTF1 );
#endif
      }
   }
   fatalErrorLoop();
   return *(Usart*) 0;
}

Stream::Status Usart::read( void* pData, uint16_t length )
{
   uint8_t* data = static_cast<uint8_t*>( pData );

   while ( length-- )
   {
      if ( !read( *data++ ) )
      {
         return Stream::ABORTED;
      }
   }
   return Stream::SUCCESS;
}

Stream::Status Usart::write( void* pData, uint16_t length )
{
   uint8_t* data = static_cast<uint8_t*>( pData );

   while ( length-- )
   {
      if ( !write( *data++ ) )
      {
         return Stream::ABORTED;
      }
   }

   return Stream::SUCCESS;
}
