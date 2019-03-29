/*
 * Logger.h
 *
 * Created: 18.06.2014 14:12:56
 * Author: viktor.pankraz
 */

#ifndef SwFramework_Traces_Logger_H
#define SwFramework_Traces_Logger_H

#include "Traces.h"
#include <DefaultTypes.h>
#include <FlashString.h>

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

      inline static Logger* getTheInstance()
      {
         return (Logger*) &theInstance;
      }

      ////    Attributes    ////

   public:

      static Stream stream;

   private:

      static Logger theInstance;
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

#endif

