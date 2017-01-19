/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: PwmOutput
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/PwmOutput.h
*********************************************************************/

#ifndef Basics_PwmOutput_H
#define Basics_PwmOutput_H

//## auto_generated
#include "Basics.h"
//## class PwmOutput
#include "PortPin.h"
//## dependency TimerCounter0
class TimerCounter0;

//## dependency TimerCounter1
class TimerCounter1;

//## package Basics

//## class PwmOutput
class PwmOutput : public PortPin {
    ////    Constructors and destructors    ////
    
public :

    //## operation PwmOutput(uint8_t,uint8_t)
    inline PwmOutput(uint8_t _portNumber, uint8_t _pinNumber) : PortPin( _portNumber, _pinNumber ) {
        //#[ operation PwmOutput(uint8_t,uint8_t)
        //#]
    }
    
    ////    Operations    ////
    
    //## operation clear()
    inline void clear();
    
    //## operation isSet()
    uint16_t isSet();
    
    //## operation set(uint8_t)
    void set(uint8_t value = 100);
};

inline void PwmOutput::clear() {
    //#[ operation clear()
    set( 0 );
    //#]
}

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/PwmOutput.h
*********************************************************************/
