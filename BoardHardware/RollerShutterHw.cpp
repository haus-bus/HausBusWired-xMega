/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: RollerShutterHw
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Electronics/HwUnitBoards/RollerShutterHw.cpp
*********************************************************************/

//## auto_generated
#include "RollerShutterHw.h"
//## dependency Logger
#include <Traces/Logger.h>
//## package Electronics::HwUnitBoards

//## class RollerShutterHw
//## class RollerShutterHw::ErrorCode

bool RollerShutterHw::isDirectionToClose() {
    //#[ operation isDirectionToClose()
    return digitalOutput0.isSet();
    //#]
}

bool RollerShutterHw::isDirectionToOpen() {
    //#[ operation isDirectionToOpen()
    return !digitalOutput0.isSet();
    //#]
}

void RollerShutterHw::off() {
    //#[ operation off()
    digitalOutput1.clear(); 
    _delay_ms( POWER_SWITCH_DELAY ); 
    
    if( digitalOutput0.isInverted() )
    {
      digitalOutput0.set();
    }
    else
    {
      digitalOutput0.clear();
    } 
    //#]
}

void RollerShutterHw::on() {
    //#[ operation on()
    digitalOutput1.set();
    _delay_ms( POWER_SWITCH_DELAY ); 
    //#]
}

void RollerShutterHw::setDirectionToClose() {
    //#[ operation setDirectionToClose()
    digitalOutput0.set();
    //#]
}

void RollerShutterHw::setDirectionToOpen() {
    //#[ operation setDirectionToOpen()
    digitalOutput0.clear();
    //#]
}

/*********************************************************************
	File Path	: AR8/release/Electronics/HwUnitBoards/RollerShutterHw.cpp
*********************************************************************/
