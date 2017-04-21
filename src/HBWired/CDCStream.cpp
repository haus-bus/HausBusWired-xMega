/* 
* CDCStream.cpp
*
* Created: 21.04.2017 00:18:42
* Author: Viktor Pankraz
*/


#include "CDCStream.h"

extern "C"
{
	#include <asf.h>
}

size_t CDCStream::write( uint8_t c )
{
	if( udi_cdc_putc( c ) )
	{
		return 1;
	}
	return 0;
}

int CDCStream::available()
{
	return udi_cdc_get_nb_received_data();	
}

int CDCStream::read()
{
	return udi_cdc_getc();
}

int CDCStream::peek()
{
	return -1;
}

void CDCStream::flush()
{
	
}