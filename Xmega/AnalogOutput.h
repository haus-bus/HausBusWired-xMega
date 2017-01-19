/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: AnalogOutput
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/AnalogOutput.h
*********************************************************************/

#ifndef Basics_AnalogOutput_H
#define Basics_AnalogOutput_H

#include "Basics.h"
#include "AnalogInput.h"
//## package Basics

//## class AnalogOutput
class AnalogOutput : public AnalogInput {
    ////    Constructors and destructors    ////
    
public :

    //## operation AnalogOutput(uint8_t,uint8_t)
    inline AnalogOutput(uint8_t _portNumber, uint8_t _pinNumber) : AnalogInput( portNumber, pinNumber ) {
        //#[ operation AnalogOutput(uint8_t,uint8_t)
        //#]
    }
    
    ////    Operations    ////
    
    //## operation clear()
    inline void clear() {
        //#[ operation clear()
        setValue( 0 ); 
        getIoPort().clearPins( getPin() );
        //#]
    }
    
    //## operation isSet()
    inline bool isSet() {
        //#[ operation isSet()
        return value;
        //#]
    }
    
    //## operation set(uint16_t)
    inline void set(uint16_t _value = MAX_VALUE) {
        //#[ operation set(uint16_t)
        setValue( _value ); 
        value ? getIoPort().setPins( getPin() ) : getIoPort().clearPins( getPin() );
        //#]
    }
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/AnalogOutput.h
*********************************************************************/
