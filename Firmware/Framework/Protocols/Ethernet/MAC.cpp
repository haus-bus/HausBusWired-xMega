/*
 * MAC.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "MAC.h"

MAC MAC::local;

Logger& Logger::operator<<( const MAC& mac )
{
   *this << mac.getAddress( 0 ) << ':' << mac.getAddress( 1 ) << ':' << mac.getAddress( 2 ) << ':'
         << mac.getAddress( 3 ) << ':' << mac.getAddress( 4 ) << ':' << mac.getAddress( 5 );
   return *this;
}