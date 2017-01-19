/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: Logger
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/SwFramework/Traces/Logger.cpp
*********************************************************************/

//## auto_generated
#include "Logger.h"
//## package SwFramework::Traces

//## class Logger
Logger::Stream Logger::stream(streamDummy);

Logger Logger::theInstance;

Logger& Logger::instance() {
    //#[ operation instance()
    return theInstance;
    /*
    static Logger* theInstance = 0;
    if( !theInstance )
      theInstance = new Logger;
      
    return *theInstance;
    */
    //#]
}

Logger& Logger::operator<<(const char* val) {
    //#[ operation operator<<(const char*)
    while( *val )
      stream( *val++ );
      
    return *this;
    //#]
}

Logger& Logger::operator<<(const uint32_t& val) {
    //#[ operation operator<<(uint32_t)
    printHex( val, sizeof(val) );
    return *this; 
    //#]
}

Logger& Logger::operator<<(uint16_t val) {
    //#[ operation operator<<(uint16_t)
    printHex( val, sizeof(val) );
    return *this; 
    //#]
}

Logger& Logger::operator<<(uint8_t val) {
    //#[ operation operator<<(uint8_t)
    printHex( val, sizeof(val) );
    return *this; 
    //#]
}

Logger& Logger::operator<<(FlashString* fString) {
    //#[ operation operator<<(FlashString)
    char c = *fString++;
    while( c ) 
    {
      stream( c );
      c = *fString++;
    }
    return *this; 
    //#]
}

void Logger::printHex(const uint32_t& val, uint8_t size) {
    //#[ operation printHex(uint32_t,uint8_t)
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
    //#]
}

/*********************************************************************
	File Path	: SwFrameworkAvr/debug/SwFramework/Traces/Logger.cpp
*********************************************************************/
