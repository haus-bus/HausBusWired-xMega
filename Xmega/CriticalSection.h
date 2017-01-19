/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: CriticalSection
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/CriticalSection.h
*********************************************************************/

#ifndef Basics_CriticalSection_H
#define Basics_CriticalSection_H

//## class CriticalSection
#include "GlobalInterrupt.h"
//## package Basics

//## class CriticalSection
class CriticalSection : public GlobalInterrupt {
    ////    Constructors and destructors    ////
    
public :

    //## operation CriticalSection()
    inline CriticalSection() {
        //#[ operation CriticalSection()
        sreg = SREG;
        disable();
        //#]
    }
    
    //## operation ~CriticalSection()
    inline ~CriticalSection() {
        //#[ operation ~CriticalSection()
        SREG = sreg;
        //#]
    }
    
    ////    Attributes    ////

private :

    unsigned char sreg;		//## attribute sreg
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/CriticalSection.h
*********************************************************************/
