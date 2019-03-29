/*
 * SwFramework.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "SwFramework.h"


uint32_t changeEndianness( const uint32_t& value )
{

   convert_u con;
   con.byte[0] = ( (uint8_t const*)&value )[3];
   con.byte[1] = ( (uint8_t const*)&value )[2];
   con.byte[2] = ( (uint8_t const*)&value )[1];
   con.byte[3] = ( (uint8_t const*)&value )[0];
   return con.dword;

}

uint16_t changeEndianness( const uint16_t& value )
{

   convert_u con;
   con.byte[0] = ( (uint8_t const*)&value )[1];
   con.byte[1] = ( (uint8_t const*)&value )[0];
   return con.word[0];

}

uint8_t dec2bcd( uint8_t num )
{
   return ( ( num / 10 * 16 ) + ( num % 10 ) );
}

uint8_t bcd2dec( uint8_t num )
{
   return ( ( num / 16 * 10 ) + ( num % 16 ) );
}
