/*
 * EventBase.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "EventBase.h"
#include "EventPkg.h"

EventBase::Queue::EventData* EventBase::Queue::events( 0 );

uint8_t EventBase::Queue::eventsInUse;

uint8_t EventBase::Queue::iRead;

uint8_t EventBase::Queue::iWrite;

uint8_t EventBase::Queue::maxEvents( 0 );

void EventBase::Queue::create( uint8_t _maxEvents )
{
   if ( !events )
   {
      events = (EventData*)allocOnce( sizeof( EventData ) * _maxEvents );
      memset( events, 0, sizeof( EventData ) * _maxEvents );
      maxEvents = _maxEvents;
      clear();
   }
}

Event* EventBase::Queue::pop()
{
   if ( eventsInUse )
   {
      Event* ev = (Event*)&events[iRead++];
      ( iRead >= maxEvents ) ? iRead = 0 : 0;
      eventsInUse--;
      return ev;
   }
   return 0;
}

bool EventBase::Queue::push( const Event& event )
{
   if ( eventsInUse < maxEvents )
   {
      memcpy( &events[iWrite++], &event, sizeof( EventData ) );
      ( iWrite >= maxEvents ) ? iWrite = 0 : 0;
      eventsInUse++;
      return true;
   }
   return false;
}