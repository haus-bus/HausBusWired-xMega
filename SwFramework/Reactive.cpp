/*
 * Reactive.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Reactive.h"
#include "Scheduler.h"

Reactive::Reactive() :
   sleepTime( 0 ), remoteAccess( true )
{
   Scheduler::addJob( this );
   *reinterpret_cast<uint8_t*>( &state ) = STARTUP;
}

Reactive* Reactive::getNextOfClass( uint8_t classId, uint8_t& startIndex )
{
   return Scheduler::getNextOfClass( classId, startIndex );
}

Reactive* Reactive::getObject( uint16_t objectId )
{
   return Scheduler::getJob( objectId );
}

Reactive** Reactive::getObjectList()
{
   return Scheduler::getItsReactive();
}

uint8_t Reactive::getMaxObjectListSize()
{
   return Scheduler::getMaxJobs();
}

Reactive::State Reactive::getState() const
{
   CriticalSection doNotInterrupt;
   return state;
}

void Reactive::terminate()
{
   Scheduler::removeJob( *this );
}


void Reactive::setGlobalSleepDelay( uint16_t delay )
{
   Scheduler::globalSleepDelay = delay;
}


void Reactive::setMainState( uint8_t mainState )
{
   CriticalSection doNotInterrupt;
   state.main = mainState;
}

void Reactive::setState( const Reactive::State& p_state )
{
   CriticalSection doNotInterrupt;
   state = p_state;
}

void Reactive::setSubState( uint8_t subState )
{
   CriticalSection doNotInterrupt;
   state.sub = subState;
}
