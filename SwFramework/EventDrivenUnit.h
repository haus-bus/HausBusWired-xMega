/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: EventDrivenUnit
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/EventDrivenUnit.h
*********************************************************************/

#ifndef SwFramework_EventDrivenUnit_H
#define SwFramework_EventDrivenUnit_H

//## auto_generated
#include "SwFramework.h"
//## class EventDrivenUnit
#include "Object.h"
//## operation notifyEvent(Event)
class Event;

//## package SwFramework

//## class EventDrivenUnit
class EventDrivenUnit : public Object {
    ////    Operations    ////
    
public :

    //## operation notifyEvent(Event)
    virtual bool notifyEvent(const Event& event) = 0;
};

#endif
/*********************************************************************
	File Path	: SwFrameworkAvr/debug/EventDrivenUnit.h
*********************************************************************/
