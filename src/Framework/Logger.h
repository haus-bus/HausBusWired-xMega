/* 
* Logger.h
*
* Created: 03.12.2016 01:21:28
* Author: haus-bus
*/


#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "FlashString.h"

class Logger
{
//variables
public:

  typedef Logger& (*Manipulator)( Logger& );

  typedef void (*Stream)( char );
  
  
protected:
private:

//functions
public:
	inline Logger() {}
		
	inline static void setStream( Stream p_stream )
	{
		stream = p_stream;
	}
		
	static Logger& instance();

	inline Logger& operator<<( char c );

	Logger& operator<<( const char* val );

	Logger& operator<<( const uint32_t& val );

	inline Logger& operator<<( const Manipulator& manipulator );

	Logger& operator<<( uint16_t val );

	Logger& operator<<( uint8_t val );

	Logger& operator<<( FlashString* fString );
		  
protected:

	void printHex( const uint32_t& val, uint8_t size );

private:

  static Stream stream;	

  static Logger theInstance;	

}; //Logger

inline Logger& Logger::operator<<( char c )
{
	stream( c );
	return *this;
}

inline Logger& Logger::operator<<( const Logger::Manipulator& manipulator )
{
	return manipulator( *this );
}

Logger& endl(Logger& me);

Logger& newTraceLine(Logger& me);

#endif //__LOGGER_H__
