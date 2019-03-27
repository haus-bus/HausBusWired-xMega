/*
 * EventBase.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_EventPkg_EventBase_H
#define SwFramework_EventPkg_EventBase_H

#include <DefaultTypes.h>
#include <MemoryManager.h>
class Event;


class EventBase
{
   public:

      class Queue;

      static const uint8_t MAX_EVENT_SIZE = 3 * sizeof( uintptr_t );

      class Queue
      {
         public:

            struct EventData {
               uint8_t data[MAX_EVENT_SIZE];
            };

            ////    Operations    ////

            inline static void clear()
            {
               eventsInUse = 0;
               iRead = 0;
               iWrite = 0;
            }

            static void create( uint8_t _maxEvents );

            inline static Event* front();

            static Event* pop();

            static bool push( const Event& event );

            ////    Attributes    ////

         private:

            static EventData* events;

            static uint8_t eventsInUse;

            static uint8_t iRead;

            static uint8_t iWrite;

            static uint8_t maxEvents;
      };

      ////    Additional operations    ////

      inline uint8_t getId() const
      {
         return id;
      }

      inline void setId( uint8_t p_id )
      {
         id = p_id;
      }

      ////    Attributes    ////

      uint8_t id;
};

inline Event* EventBase::Queue::front()
{
   if ( eventsInUse )
   {
      return (Event*)&events[iRead];
   }
   return 0;
}

#endif
