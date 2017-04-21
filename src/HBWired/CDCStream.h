/* 
* CDCStream.h
*
* Created: 21.04.2017 00:18:42
* Author: Viktor Pankraz
*/


#ifndef __CDCSTREAM_H__
#define __CDCSTREAM_H__

#include "Arduino/Stream.h"


class CDCStream : public Stream
{
//variables
public:
protected:
private:

//functions
public:
	// realize class Print
	virtual size_t write(uint8_t);
	
	// realize class Stream
	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();

protected:
private:


}; //CDCStream

#endif //__CDCSTREAM_H__
