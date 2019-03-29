/*
 * Logger.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Logger.h"

Logger::Stream Logger::stream( streamDummy );

Logger Logger::theInstance;

Logger& Logger::instance()
{
   return theInstance;
   /*
      static Logger* theInstance = 0;
      if( !theInstance )
      theInstance = new Logger;

      return *theInstance;
    */
}

Logger& Logger::operator<<( const char* val )
{
   while ( *val )
   {
      stream( *val++ );
   }

   return *this;
}

Logger& Logger::operator<<( const uint32_t& val )
{
   printHex( val, sizeof( val ) );
   return *this;
}

Logger& Logger::operator<<( uint16_t val )
{
   printHex( val, sizeof( val ) );
   return *this;
}

Logger& Logger::operator<<( uint8_t val )
{
   printHex( val, sizeof( val ) );
   return *this;
}

Logger& Logger::operator<<( FlashString* fString )
{
   char c = *fString++;
   while ( c )
   {
      stream( c );
      c = *fString++;
   }
   return *this;
}

void Logger::printHex( const uint32_t& val, uint8_t size )
{
   size <<= 3;
   bool leadingZeros = true;

   do
   {
      size -= 4;
      char c = static_cast<char>( ( val >> size ) & 0xF );
      if ( leadingZeros && !c && size )
      {
         continue;
      }

      leadingZeros = false;
      c += '0';
      if ( c > '9' )
      {
         c += 'A' - '0' - 10;
      }
      stream( c );
   }
   while ( size );
}