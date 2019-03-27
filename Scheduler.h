/*
 * Scheduler.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Scheduler_H
#define SwFramework_Scheduler_H

#include "SwFramework.h"
#include "DefaultTypes.h"
#include "Tracing/Logger.h"
#include "Reactive.h"

class Event;

class SystemTime;

class Scheduler
{
   ////    Operations    ////

   public:

      static bool addJob( Reactive* newJob );

      static Reactive* getJob( uint16_t _id );

      static Reactive* getNextOfClass( uint8_t classId, uint8_t& startIndex );

      static void removeJob( const Reactive& job );

      static void runJobs();

      static void setup( uint8_t _maxJobs, uint8_t _maxEvents = 32 );

      ////    Additional operations    ////

      inline static Reactive** getItsReactive()
      {
         return itsReactive;
      }

      inline static void setItsReactive( Reactive** p_itsReactive )
      {
         itsReactive = p_itsReactive;
      }

      inline static uint8_t getMaxJobs()
      {
         return maxJobs;
      }

      inline static void setMaxJobs( uint8_t p_maxJobs )
      {
         maxJobs = p_maxJobs;
      }

      inline static const char* getId()
      {
         return "Scheduler ";
      }

      ////    Attributes    ////

      static Reactive** itsReactive;

      static uint8_t maxJobs;

      static uint16_t globalSleepDelay;

      static const uint8_t debugLevel;
};

#endif
