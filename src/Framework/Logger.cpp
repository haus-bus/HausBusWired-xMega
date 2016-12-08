/* 
* Logger.cpp
*
* Created: 03.12.2016 01:21:28
* Author: haus-bus
*/


#include "Logger.h"
#include "SystemTime.h"

Logger& endl(Logger& me) 
{
	return me << '\n';
}

Logger& newTraceLine(Logger& me) 
{
	return me << '\n' << '>' << SystemTime::now() << ' ';
}

void streamDummy(char c) {}

Logger::Stream Logger::stream(streamDummy);

Logger Logger::theInstance;

Logger& Logger::instance() 
{
    return theInstance;
}

Logger& Logger::operator<<(const char* val) 
{
    while( *val )
      stream( *val++ );
      
    return *this;
}

Logger& Logger::operator<<(const uint32_t& val) 
{
    printHex( val, sizeof(val) );
    return *this; 
}

Logger& Logger::operator<<(uint16_t val) 
{
    printHex( val, sizeof(val) );
    return *this; 
}

Logger& Logger::operator<<(uint8_t val) 
{
    printHex( val, sizeof(val) );
    return *this; 
}

Logger& Logger::operator<<(FlashString* fString) 
{
    char c = *fString++;
    while( c ) 
    {
      stream( c );
      c = *fString++;
    }
    return *this; 
}

void Logger::printHex(const uint32_t& val, uint8_t size) 
{
    size <<= 3;
    bool leadingZeros  = true;
    
    do
    { 
      size-=4;   
      char c = static_cast<char>( (val>>size)&0xF );
      if( leadingZeros && !c && size )
        continue;
      
      leadingZeros = false;
      c += '0';
      if( c > '9' )
        c += 'A'-'0'-10;
      stream( c );
    }while( size );
}

