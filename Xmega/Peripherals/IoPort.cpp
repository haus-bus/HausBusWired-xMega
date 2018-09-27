/*
 * IoPort.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "IoPort.h"

IoPort IoPort::dummyPort;

IoPort& IoPort::instance( uint8_t portNumber )
{

   switch ( portNumber )
   {
#ifdef PORTA
      case PortA:
         return *reinterpret_cast<IoPort*>( &PORTA );
#endif

#ifdef PORTB
      case PortB:
         return *reinterpret_cast<IoPort*>( &PORTB );
#endif

#ifdef PORTC
      case PortC:
         return *reinterpret_cast<IoPort*>( &PORTC );
#endif

#ifdef PORTD
      case PortD:
         return *reinterpret_cast<IoPort*>( &PORTD );
#endif

#ifdef PORTE
      case PortE:
         return *reinterpret_cast<IoPort*>( &PORTE );
#endif

#ifdef PORTF
      case PortF:
         return *reinterpret_cast<IoPort*>( &PORTF );
#endif

#ifdef PORTR
      case PortR:
         return *reinterpret_cast<IoPort*>( &PORTR );
#endif
   }
   return dummyPort;
}
