/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: PwmOutput
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/PwmOutput.cpp
*********************************************************************/

//## auto_generated
#include "PwmOutput.h"
//## dependency TimerCounter0
#include "Peripherals/TimerCounter0.h"
//## dependency TimerCounter1
#include "Peripherals/TimerCounter1.h"
//## package Basics

//## class PwmOutput
uint16_t PwmOutput::isSet() {
    //#[ operation isSet()
    TimerCounter0* tc0 = &TimerCounter0::instance( portNumber ); 
    if( tc0 && ( pinNumber < Pin4 ) )
    {
      if     ( pinNumber == 0 ) return tc0->getCaptureA();
      else if( pinNumber == 1 ) return tc0->getCaptureB();  
      else if( pinNumber == 2 ) return tc0->getCaptureC();
      else                      return tc0->getCaptureD(); 
    }   
    
    TimerCounter1* tc1 = &TimerCounter1::instance( portNumber ); 
    if( tc1 && ( pinNumber >= Pin4 ) )
    {
      if     ( pinNumber == 4 ) tc1->getCaptureA();
      else if( pinNumber == 5 ) tc1->getCaptureB(); 
    }
    return 0;
    //#]
}

void PwmOutput::set(uint8_t value) {
    //#[ operation set(uint8_t)
    TimerCounter0* tc0 = &TimerCounter0::instance( portNumber ); 
    if( tc0 && ( pinNumber < Pin4 ) )
    {
      if     ( pinNumber == 0 ) tc0->setCompareA( value );
      else if( pinNumber == 1 ) tc0->setCompareB( value );   
      else if( pinNumber == 2 ) tc0->setCompareC( value );
      else                      tc0->setCompareD( value ); 
      return;
    }   
    
    TimerCounter1* tc1 = &TimerCounter1::instance( portNumber ); 
    if( tc1 && ( pinNumber >= Pin4 ) )
    {
      if     ( pinNumber == 4 ) tc1->setCompareA( value );
      else if( pinNumber == 5 ) tc1->setCompareB( value ); 
    }
    
    //#]
}

/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/PwmOutput.cpp
*********************************************************************/
