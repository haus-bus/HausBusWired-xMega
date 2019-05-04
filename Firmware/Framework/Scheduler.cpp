/*
 * Scheduler.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Scheduler.h"
#include "MemoryManager.h"
#include "EventPkg/EventPkg.h"
#include "Time/SystemTime.h"

const uint8_t Scheduler::debugLevel( DEBUG_LEVEL_OFF );

Reactive** Scheduler::itsReactive = 0;

uint8_t Scheduler::maxJobs( 0 );

uint16_t Scheduler::globalSleepDelay( 0 );

bool Scheduler::addJob( Reactive* newJob )
{
   if ( itsReactive )
   {
      for ( uint8_t i = 0; i < maxJobs; i++ )
      {
         Reactive* job = itsReactive[i];
         if ( !job )
         {
            itsReactive[i] = newJob;
            // newJob.setSleepTime(0);
            // newJob.setMainState( Job::STARTUP );
            return true;
         }
      }
   }
   return false;
}

Reactive* Scheduler::getJob( uint16_t _id )
{
   for ( uint8_t i = 0; i < maxJobs; i++ )
   {
      if ( itsReactive[i] && ( itsReactive[i]->getId() == _id ) )
      {
         return itsReactive[i];
      }
   }
   return 0;
}

Reactive* Scheduler::getNextOfClass( uint8_t classId, uint8_t& startIndex )
{
   for (; startIndex < maxJobs; startIndex++ )
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
}

void Scheduler::removeJob( const Reactive& job )
{
   if ( itsReactive )
   {
      for ( uint8_t i = 0; i < maxJobs; i++ )
      {
         if ( itsReactive[i] == &job )
         {
            // copy all elements behind the found element
            // one position in frontside direction
            for (; i < maxJobs - 1; i++ )
            {
               itsReactive[i] = itsReactive[i + 1];
            }
            itsReactive[i] = 0;
         }
      }
   }
}

void Scheduler::runJobs()
{
   static SystemTime::time_t time = SystemTime::now();
   SystemTime::time_t elapsedTime = SystemTime::since( time );
   time = SystemTime::now();
   if ( elapsedTime > 100 )
   {
      WARN_2( "mainLoopTooLong: ", elapsedTime )
   }

   for ( uint8_t i = 0; i < maxJobs; i++ )
   {
      notifyIdle();
      if ( itsReactive[i] )
      {
         uint16_t sleepTime = itsReactive[i]->getSleepTime();
         if ( sleepTime == Reactive::NO_WAKE_UP )
         {
            continue;
         }

         if ( sleepTime > elapsedTime )
         {
            itsReactive[i]->setSleepTime( sleepTime - elapsedTime );
         }
         else
         {
            itsReactive[i]->setSleepTime( 0 );
            notifyBusy();
            SystemTime::time_t jobStart = SystemTime::now();
            DEBUG_H2( "run job ", (uintptr_t)itsReactive[i] );
            evWakeup( itsReactive[i] ).send();
            DEBUG_H4( "job ", (uintptr_t)itsReactive[i], " returns after 0x", SystemTime::since( jobStart ) );
            if ( SystemTime::since( jobStart ) > 20 )
            {
               WARN_4( "job: 0x", itsReactive[i]->getId(), " needs too long: 0x", SystemTime::since( jobStart ) );
            }
         }
      }

      Event ev;
      while ( Event::messageQueue.pop( ev ) )
      {
         DEBUG_H4( "send event id:", ev.getId(), " dest: ", (uintptr_t) ev.getDestination() );
         ev.send();
         DEBUG_H3( "event id:", ev.getId(), " processed" );
      }
   }
}

void Scheduler::setup( uint8_t _maxJobs, uint8_t _maxEvents )
{
   itsReactive = (Reactive**) allocOnce( sizeof( itsReactive ) * _maxJobs );
   memset( itsReactive, 0, sizeof( itsReactive ) * _maxJobs );
   maxJobs = _maxJobs;
}

