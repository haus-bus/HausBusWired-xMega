/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: GlobalInterrupt
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/GlobalInterrupt.h
*********************************************************************/

#ifndef Basics_GlobalInterrupt_H
#define Basics_GlobalInterrupt_H

#include <avr/io.h>
//## package Basics

//## class GlobalInterrupt
class GlobalInterrupt {
    ////    Operations    ////
    
public :

    //## operation disable()
    inline static void disable() {
        //#[ operation disable()
        __asm__ __volatile__ ("cli" :: );
        //#]
    }
    
    //## operation enable()
    inline static void enable() {
        //#[ operation enable()
        __asm__ __volatile__ ("sei" :: );
        //#]
    }
    
    //## operation waitForInterrupt()
    inline static void waitForInterrupt() {
        //#[ operation waitForInterrupt()
        __asm__ __volatile__ ("sleep");
        //#]
    }
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/GlobalInterrupt.h
*********************************************************************/
