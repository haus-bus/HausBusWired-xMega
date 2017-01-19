/********************************************************************
 Rhapsody	: 8.0.3
 Login		: viktor.pankraz
 Component	: SwFrameworkAvr
 Configuration 	: debug
 Model Element	: Scheduler
 //!	Generated Date	: Wed, 18, Jun 2014
 File Path	: SwFrameworkAvr/debug/Scheduler.cpp
 *********************************************************************/

//## auto_generated
#include "Scheduler.h"
#include "EventPkg/EventPkg.h"
#include <MemoryManager.h>
#include <SystemTime.h>

Reactive** Scheduler::itsReactive = 0;

uint8_t Scheduler::maxJobs( 0 );

uint16_t Scheduler::globalSleepDelay( 0 );

bool Scheduler::addJob( Reactive* newJob )
{
  //#[ operation addJob(Reactive)
  if ( itsReactive )
  {
    for ( uint8_t i = 0; i < maxJobs; i++ )
    {
      Reactive* job = itsReactive[i];
      if ( !job )
      {
        itsReactive[i] = newJob;
        //newJob.setSleepTime(0);
        //newJob.setMainState( Job::STARTUP );
        return true;
      }
    }
  }
  return false;
  //#]
}

Reactive* Scheduler::getJob( uint16_t _id )
{
  //#[ operation getJob(uint16_t)
  for ( uint8_t i = 0; i < maxJobs; i++ )
  {
    if ( itsReactive[i] && (itsReactive[i]->getId() == _id) )
    {
      return itsReactive[i];
    }
  }
  return 0;
  //#]
}

Reactive* Scheduler::getNextOfClass( uint8_t classId, uint8_t& startIndex )
{
  //#[ operation getNextOfClass(uint8_t,uint8_t)
  for ( ; startIndex < maxJobs; startIndex++ )
  {
    if ( itsReactive[startIndex] )
    {
      if ( itsReactive[startIndex]->isClassId( classId ) )
      {
        return itsReactive[startIndex++];
      }
    }
  }
  return 0;
  //#]
}

void Scheduler::removeJob( const Reactive& job )
{
  //#[ operation removeJob(Reactive)
  if ( itsReactive )
  {
    for ( uint8_t i = 0; i < maxJobs; i++ )
    {
      if ( itsReactive[i] == &job )
      {
        // copy all elements behind the found element
        // one position in frontside direction
        for ( ; i < maxJobs - 1; i++ )
        {
          itsReactive[i] = itsReactive[i + 1];
        }
        itsReactive[i] = 0;
      }
    }
  }
  //#]
}

void Scheduler::runJobs()
{
  //#[ operation runJobs()
  SystemTime::time_t elapsedTime, time = SystemTime::now();

  while ( isRunning() )
  {
    elapsedTime = SystemTime::since( time );
    time = SystemTime::now();
    if ( elapsedTime > 500 )
    {
      WARN_2( "mainLoopTooLong: ", elapsedTime )
    }

    for ( uint8_t i = 0; i < maxJobs; i++ )
    {
      notifyIdle();
      if ( itsReactive[i] )
      {
        uint16_t sleepTime = itsReactive[i]->getSleepTime();
        if ( sleepTime == Reactive::NO_WAKE_UP ) continue;

        if ( sleepTime > elapsedTime )
        {
          itsReactive[i]->setSleepTime( sleepTime - elapsedTime );
        }
        else
        {
          itsReactive[i]->setSleepTime( 0 );
          notifyBusy();
          //WARN_2( "call: 0x", itsReactive[i]->getId() );
          evWakeup( itsReactive[i] ).send();
        }
      }
    }

    Event ev;
    bool notEmpty = Event::messageQueue.pop( ev );
    while ( notEmpty )
    {
      ev.send();
      notEmpty = Event::messageQueue.pop( ev );
    }
  }
}

void Scheduler::setup( uint8_t _maxJobs, uint8_t _maxEvents )
{
  itsReactive = (Reactive**) allocOnce( sizeof(itsReactive) * _maxJobs );
  memset( itsReactive, 0, sizeof(itsReactive) * _maxJobs );
  maxJobs = _maxJobs;
}

