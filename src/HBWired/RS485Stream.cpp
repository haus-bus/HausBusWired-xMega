/* 
* CDCStream.cpp
*
* Created: 21.04.2017 00:18:42
* Author: Viktor Pankraz
*/


#include "RS485Stream.h"


RS485Stream::RS485Stream( usart_if _serial )
{
	serial = _serial;
}

size_t RS485Stream::write( uint8_t c )
{
	if( usart_putchar( serial, c ) == STATUS_OK )
	{
		return 1;
	}
	return 0;
}

int RS485Stream::available()
{
	return usart_rx_is_complete( serial );	
}

int RS485Stream::read()
{
	return usart_getchar(serial); 
}

int RS485Stream::peek()
{
	return -1;
}

void RS485Stream::flush()
{
	usart_clear_tx_complete( serial );
	while( !usart_tx_is_complete( serial ) );
}