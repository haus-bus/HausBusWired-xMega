/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: NonCriticalSection
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/NonCriticalSection.h
*********************************************************************/

#ifndef Basics_NonCriticalSection_H
#define Basics_NonCriticalSection_H

//## auto_generated
#include "Basics.h"
//## class NonCriticalSection
#include "GlobalInterrupt.h"
//## package Basics

//## class NonCriticalSection
class NonCriticalSection : public GlobalInterrupt {
    ////    Constructors and destructors    ////
    
public :

    //## operation NonCriticalSection()
    inline NonCriticalSection() {
        //#[ operation NonCriticalSection()
        sreg = SREG;
        enable();
        //#]
    }
    
    //## operation ~NonCriticalSection()
    inline ~NonCriticalSection() {
        //#[ operation ~NonCriticalSection()
        SREG = sreg;
        //#]
    }
    
    ////    Attributes    ////

private :

    unsigned char sreg;		//## attribute sreg
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/NonCriticalSection.h
*********************************************************************/
