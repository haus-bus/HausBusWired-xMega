/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: AR8System
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Systems/HomeAutomationSystems/AR8System.h
*********************************************************************/

#ifndef Systems_HomeAutomationSystems_AR8System_H
#define Systems_HomeAutomationSystems_AR8System_H

#include "AR8SystemHw.h"

#include <DigitalPort.h>
#include <HomeAutomation.h>

class DebugOptions;

//## dependency Gateway
class Gateway;

//## dependency MyEvent
class MyEvent;

//## dependency PersistentRules
class PersistentRules;

//## dependency RuleEngine
class RuleEngine;

//## dependency Scheduler
class Scheduler;

//## dependency WeekTime
class WeekTime;

//## package Systems::HomeAutomationSystems

//## class AR8System
class AR8System : public HomeAutomation {
    ////    Constructors and destructors    ////
    
public :

    //## operation AR8System()
    AR8System();
    
    ////    Operations    ////
    
    //## operation start()
    static void start();
    
    ////    Additional operations    ////
    
    //## auto_generated
    DigitalPort* getDigitalPortE() const;
    
    //## auto_generated
    DigitalPort* getDigitalPortF() const;
    
    //## auto_generated
    AR8SystemHw* getHardware() const;
    
    ////    Relations and components    ////

protected :

    DigitalPort digitalPortE;		//## classInstance digitalPortE
    
    DigitalPort digitalPortF;		//## classInstance digitalPortF
    
    AR8SystemHw hardware;		//## classInstance hardware
};

#endif
/*********************************************************************
	File Path	: AR8/release/Systems/HomeAutomationSystems/AR8System.h
*********************************************************************/
