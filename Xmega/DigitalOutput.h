/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: DigitalOutput
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/DigitalOutput.h
*********************************************************************/

#ifndef Basics_DigitalOutput_H
#define Basics_DigitalOutput_H

//## auto_generated
#include "Basics.h"
//## class DigitalOutput
#include "DigitalInput.h"
//## package Basics

//## class DigitalOutput
class DigitalOutput : public DigitalInput {
    ////    Constructors and destructors    ////
    
public :

    //## operation DigitalOutput(uint8_t,uint8_t)
    inline DigitalOutput(uint8_t _portNumber, uint8_t _pinNumber) : DigitalInput( _portNumber, _pinNumber ) {
        //#[ operation DigitalOutput(uint8_t,uint8_t)
        //getIoPort().setPinsAsOutput( getPin() );
        //#]
    }
    
    ////    Operations    ////
    
    //## operation clear()
    void clear();
    
    //## operation set()
    void set();
    
    //## operation toggle()
    void toggle();
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/DigitalOutput.h
*********************************************************************/
