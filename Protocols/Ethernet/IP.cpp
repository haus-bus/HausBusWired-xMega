/*
 * IP.cpp
 *
 *  Created on: 06.08.2014
 *      Author: Viktor Pankraz
 */

#include "IP.h"

IP IP::local = { .address = IP_TO_UINT32( 192, 168, 178, 254 ) };

IP IP::netmask = { .address = IP_TO_UINT32( 255, 255, 255, 0 ) };

IP IP::router = { .address = IP_TO_UINT32( 192, 168, 178, 1 ) };

IP IP::defaultIp = { .address = IP_TO_UINT32( 192, 168, 178, 254 ) };

IP IP::defaultNetmask = { .address = IP_TO_UINT32( 255, 255, 255, 0 ) };

IP IP::defaultRouterIp = { .address = IP_TO_UINT32( 192, 168, 178, 1 ) };


Logger& Logger::operator<<( const IP& ip )
{
   *this << LBYTE( LWORD( ip.address ) ) << '.' << HBYTE( LWORD( ip.address ) ) << '.' << LBYTE( HWORD( ip.address ) ) << '.' << HBYTE( HWORD( ip.address ) );
   return *this;
}