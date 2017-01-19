/*********************************************************************
 Rhapsody	: 8.0.3
 Login		: viktor.pankraz
 Component	: SwFrameworkAvr
 Configuration 	: debug
 Model Element	: Logger
 //!	Generated Date	: Wed, 18, Jun 2014
 File Path	: SwFrameworkAvr/debug/SwFramework/Traces/Logger.h
 *********************************************************************/

#ifndef SwFramework_Traces_Logger_H
#define SwFramework_Traces_Logger_H

#include "Traces.h"
#include "DefaultTypes.h"
#include <FlashString.h>

class Logger
{
public:

  typedef Logger& (*Manipulator)( Logger& );

  typedef void (*Stream)( char );

  ////    Constructors and destructors    ////

  //## operation Logger()
  inline Logger()
  {
    //#[ operation Logger()
    //#]
  }

  ////    Operations    ////

  //## operation instance()
  static Logger& instance();

  //## operation operator<<(char)
  inline Logger& operator<<( char c );

  //## operation operator<<(const char*)
  Logger& operator<<( const char* val );

  //## operation operator<<(uint32_t)
  Logger& operator<<( const uint32_t& val );

  //## operation operator<<(Manipulator)
  inline Logger& operator<<( const Manipulator& manipulator );

  //## operation operator<<(uint16_t)
  Logger& operator<<( uint16_t val );

  //## operation operator<<(uint8_t)
  Logger& operator<<( uint8_t val );

  //## operation operator<<(FlashString)
  Logger& operator<<( FlashString* fString );

  //## operation printHex(uint32_t,uint8_t)
  void printHex( const uint32_t& val, uint8_t size );

  ////    Additional operations    ////

  //## auto_generated
  inline static Stream getStream()
  {
    //#[ auto_generated
    return stream;
    //#]
  }

  //## auto_generated
  inline static void setStream( Stream p_stream )
  {
    //#[ auto_generated
    stream = p_stream;
    //#]
  }

private:

  //## auto_generated
  inline static Logger* getTheInstance()
  {
    //#[ auto_generated
    return (Logger*) &theInstance;
    //#]
  }

  ////    Attributes    ////

public:

  static Stream stream;		//## attribute stream

private:

  static Logger theInstance;		//## attribute theInstance
};

inline Logger& Logger::operator<<( char c )
{
  //#[ operation operator<<(char)
  stream( c );
  return *this;
  //#]
}

inline Logger& Logger::operator<<( const Logger::Manipulator& manipulator )
{
  //#[ operation operator<<(Manipulator)
  return manipulator( *this );
  //#]
}

#endif
/*********************************************************************
 File Path	: SwFrameworkAvr/debug/SwFramework/Traces/Logger.h
 *********************************************************************/
