/* 
* CDCStream.h
*
* Created: 21.04.2017 00:18:42
* Author: Viktor Pankraz
*/


#ifndef __SERIALSTREAM_H__
#define __SERIALSTREAM_H__

extern "C"
{
	#include <asf.h>
}

#include <Stream.h>

class SerialStream : public Stream
{
//variables
public:
protected:
private:
	usart_if serial;

//functions
public:
	SerialStream( usart_if );

	// realize class Print
	virtual size_t write(uint8_t);
	
	// realize class Stream
	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();

protected:
private:


}; //SerialStream

#endif //__SERIALSTREAM_H__
