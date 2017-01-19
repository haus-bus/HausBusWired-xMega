/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: DigitalOutput
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/DigitalOutput.cpp
*********************************************************************/

//## auto_generated
#include "DigitalOutput.h"
//## package Basics

//## class DigitalOutput
void DigitalOutput::clear() {
    //#[ operation clear()
    getIoPort().clearPins( getPin() );
    //#]
}

void DigitalOutput::set() {
    //#[ operation set()
    getIoPort().setPins( getPin() );
    //#]
}

void DigitalOutput::toggle() {
    //#[ operation toggle()
    getIoPort().togglePins( getPin() );
    //#]
}

/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/DigitalOutput.cpp
*********************************************************************/
