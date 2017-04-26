/* 
* SerialStream.cpp
*
* Created: 21.04.2017 00:18:42
* Author: Viktor Pankraz
*/


#include "SerialStream.h"


SerialStream::SerialStream( usart_if _serial )
{
	serial = _serial;
}

size_t SerialStream::write( uint8_t c )
{
	if( usart_putchar( serial, c ) == STATUS_OK )
	{
		return 1;
	}
	return 0;
}

int SerialStream::available()
{
	return usart_rx_is_complete( serial );	
}

int SerialStream::read()
{
	return usart_getchar(serial); 
}

int SerialStream::peek()
{
	return -1;
}

void SerialStream::flush()
{
	usart_clear_tx_complete( serial );
	while( !usart_tx_is_complete( serial ) );
}