/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: Traces
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/SwFramework/Traces/Traces.cpp
*********************************************************************/

//## auto_generated
#include "Traces.h"
//## operation endl(Logger)
#include "Logger.h"
//## dependency SystemTime
#include <SystemTime.h>
//## package SwFramework::Traces


//## operation endl(Logger)
Logger& endl(Logger& me) {
    //#[ operation endl(Logger)
    return me << '\n';
    //#]
}

//## operation newTraceLine(Logger)
Logger& newTraceLine(Logger& me) {
    //#[ operation newTraceLine(Logger)
    return me << '\n' << '>' << SystemTime::now() << ' ';
    //#]
}

//## operation streamDummy(char)
void streamDummy(char c) {
    //#[ operation streamDummy(char)
    //#]
}

/*********************************************************************
	File Path	: SwFrameworkAvr/debug/SwFramework/Traces/Traces.cpp
*********************************************************************/
