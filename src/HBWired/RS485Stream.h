/* 
* CDCStream.h
*
* Created: 21.04.2017 00:18:42
* Author: Viktor Pankraz
*/


#ifndef __RS485STREAM_H__
#define __RS485STREAM_H__

#include "Arduino/Stream.h"

extern "C"
{
	#include <asf.h>
}

class RS485Stream : public Stream
{
//variables
public:
protected:
private:
	usart_if serial;

//functions
public:
	RS485Stream( usart_if );

	// realize class Print
	virtual size_t write(uint8_t);
	
	// realize class Stream
	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();

protected:
private:


}; //RS485Stream

#endif //__RS485STREAM_H__
