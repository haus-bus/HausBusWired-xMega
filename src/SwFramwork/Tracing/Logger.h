/*
 * Logger.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Tracing_Logger_H
#define SwFramework_Tracing_Logger_H

#include <FlashString.h>

class FlashString;

class Logger
{
   public:

      typedef Logger& ( *Manipulator )( Logger& );

      typedef void (* Stream)( char );

      ////    Constructors and destructors    ////
      inline Logger()
      {

      }

      ////    Operations    ////

      static Logger& instance();

      inline Logger& operator<<( char c );

      Logger& operator<<( const char* val );

      Logger& operator<<( const uint32_t& val );

      inline Logger& operator<<( const Manipulator& manipulator );

      Logger& operator<<( uint16_t val );

      Logger& operator<<( uint8_t val );

      Logger& operator<<( FlashString* fString );

      void printHex( const uint32_t& val, uint8_t size );

      ////    Additional operations    ////

      inline static Stream getStream()
      {
         return stream;
      }

      inline static void setStream( Stream p_stream )
      {
         stream = p_stream;
      }

   private:

      ////    Attributes    ////

   public:

      static Stream stream;

};

inline Logger& Logger::operator<<( char c )
{
   stream( c );
   return *this;
}

inline Logger& Logger::operator<<( const Logger::Manipulator& manipulator )
{
   return manipulator( *this );
}

Logger& endl( Logger& me );

Logger& newTraceLine( Logger& me );

#endif

