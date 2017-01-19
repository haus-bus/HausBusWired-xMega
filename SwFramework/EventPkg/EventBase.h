/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: EventBase
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/SwFramework/EventPkg/EventBase.h
*********************************************************************/

#ifndef SwFramework_EventPkg_EventBase_H
#define SwFramework_EventPkg_EventBase_H

//## attribute id
#include <DefaultTypes.h>
//## dependency MemoryManager
#include <MemoryManager.h>
//## operation push(Event)
class Event;

//## package SwFramework::EventPkg

//## class EventBase
class EventBase {
public :

    //## auto_generated
    class Queue;
    
    //#[ type MAX_EVENT_SIZE
    static const uint8_t MAX_EVENT_SIZE = 3*sizeof(uintptr_t);
    //#]
    
    //## class EventBase::Queue
    class Queue {
    public :
    
        //## type EventData
        struct EventData {
            uint8_t data[MAX_EVENT_SIZE];		//## attribute data
        };
        
        ////    Operations    ////
        
        //## operation clear()
        inline static void clear() {
            //#[ operation clear()
            eventsInUse = 0;
            iRead = 0;
            iWrite = 0;  
            //#]
        }
        
        //## operation create(uint8_t)
        static void create(uint8_t _maxEvents);
        
        //## operation front()
        inline static Event* front();
        
        //## operation pop()
        static Event* pop();
        
        //## operation push(Event)
        static bool push(const Event& event);
        
        ////    Attributes    ////
    
    private :
    
        static EventData* events;		//## attribute events
        
        static uint8_t eventsInUse;		//## attribute eventsInUse
        
        static uint8_t iRead;		//## attribute iRead
        
        static uint8_t iWrite;		//## attribute iWrite
        
        static uint8_t maxEvents;		//## attribute maxEvents
    };
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline uint8_t getId() const {
        //#[ auto_generated
        return id;
        //#]
    }
    
    //## auto_generated
    inline void setId(uint8_t p_id) {
        //#[ auto_generated
        id = p_id;
        //#]
    }
    
    ////    Attributes    ////
    
    uint8_t id;		//## attribute id
};

inline Event* EventBase::Queue::front() {
    //#[ operation front()
    if( eventsInUse )
    {
      return (Event*)&events[iRead];
    }
    return 0;
    //#]
}

#endif
/*********************************************************************
	File Path	: SwFrameworkAvr/debug/SwFramework/EventPkg/EventBase.h
*********************************************************************/
