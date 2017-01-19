/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: TimerCounter0
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/TimerCounter0.cpp
*********************************************************************/

//## auto_generated
#include "Peripherals/TimerCounter0.h"
//## package Peripherals

//## class TimerCounter0
TimerCounter0& TimerCounter0::instance(uint8_t portNumber) {
    //#[ operation instance(uint8_t)
    switch( portNumber )
    {         
    #ifdef TCC0
      case PortC:   
        return *reinterpret_cast<TimerCounter0*> (&TCC0);  
    #endif  
    
    #ifdef TCD0    
      case PortD:   
        return *reinterpret_cast<TimerCounter0*> (&TCD0); 
    #endif  
    
    #ifdef TCE0    
      case PortE:
        return *reinterpret_cast<TimerCounter0*> (&TCE0); 
    #endif 
    
    #ifdef TCF0    
      case PortF:
        return *reinterpret_cast<TimerCounter0*> (&TCF0); 
    #endif 
              
    }  
    
    return *(TimerCounter0*)0; 
    //#]
}

/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/TimerCounter0.cpp
*********************************************************************/
