/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: EventBase
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/SwFramework/EventPkg/EventBase.cpp
*********************************************************************/

//## auto_generated
#include "EventBase.h"
//## operation push(Event)
#include "EventPkg.h"
//## package SwFramework::EventPkg

//## class EventBase
//## class EventBase::Queue
EventBase::Queue::EventData* EventBase::Queue::events(0);

uint8_t EventBase::Queue::eventsInUse;

uint8_t EventBase::Queue::iRead;

uint8_t EventBase::Queue::iWrite;

uint8_t EventBase::Queue::maxEvents(0);

void EventBase::Queue::create(uint8_t _maxEvents) {
    //#[ operation create(uint8_t)
    if( !events )
    {
      events = (EventData*)allocOnce( sizeof(EventData)*_maxEvents );    
      memset( events, 0, sizeof(EventData)*_maxEvents );
      maxEvents = _maxEvents;
      clear();
    }
    //#]
}

Event* EventBase::Queue::pop() {
    //#[ operation pop()
    if( eventsInUse )
    {   
      Event* ev = (Event*)&events[iRead++];
      ( iRead >= maxEvents ) ? iRead = 0 : 0 ;  
      eventsInUse--; 
      return ev;
    }
    return 0;
    //#]
}

bool EventBase::Queue::push(const Event& event) {
    //#[ operation push(Event)
    if( eventsInUse < maxEvents )
    {      
      memcpy( &events[iWrite++], &event, sizeof(EventData) );
      ( iWrite >= maxEvents ) ? iWrite = 0 : 0 ;
      eventsInUse++; 
      return true;
    }
    return false;
    //#]
}

/*********************************************************************
	File Path	: SwFrameworkAvr/debug/SwFramework/EventPkg/EventBase.cpp
*********************************************************************/
