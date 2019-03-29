/*
 * FlashString.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef FlashString_H
#define FlashString_H

#ifdef __AVR
#include <avr/pgmspace.h>

class FlashString
{
   public:

#define FSTR( s ) ( __extension__( { static char __c[] __attribute__( ( section( ".progmem.flashString" ) ) ) = ( s ); reinterpret_cast<FlashString*>( &__c[0] ); } ) )

      ////    Operations    ////

      inline operator char() const;
};

inline FlashString::operator char() const
{
   return pgm_read_byte( reinterpret_cast<uint16_t>( this ) );
}

#else

class FlashString
{
   public:

#define FSTR( s ) (FlashString*)s

      ////    Operations    ////

      inline operator char() const;
};

inline FlashString::operator char() const
{
   return *( (char*)this );
}

#endif // __AVR
#endif
