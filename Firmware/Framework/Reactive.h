/*
 * Reactive.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Reactive_H
#define SwFramework_Reactive_H

#include "SwFramework.h"
#include <CriticalSection.h>
#include "DefaultTypes.h"
#include "EventDrivenUnit.h"

class Scheduler;

class Reactive : public EventDrivenUnit
{
   public:

      static const uint16_t WAKE_UP = 0;
      static const uint16_t NO_WAKE_UP = 0xFFFF;

      enum States
      {
         STARTUP,
         STANDBY,
         IDLE,
         RUNNING,
         FAILTURE,
         FATAL_ERROR
      };

      struct State
      {
         uint8_t main : 3;
         uint8_t sub : 5;
      };

      ////    Constructors and destructors    ////

      Reactive();

      ////    Operations    ////

      Reactive* getNextOfClass( uint8_t classId, uint8_t& startIndex );

      Reactive* getObject( uint16_t objectId );

      Reactive** getObjectList();

      uint8_t getMaxObjectListSize();

      State getState() const;

      inline bool inFailture()
      {
         return state.main == FAILTURE;
      }

      inline bool inFatalError()
      {
         return ( state.main == FATAL_ERROR );
      }

      inline bool inIdle()
      {
         return ( state.main == IDLE );
      }

      inline bool inRunning()
      {
         return ( state.main == RUNNING );
      }

      inline bool inStandby()
      {
         return ( state.main == STANDBY );
      }

      inline bool inStartUp()
      {
         return ( state.main == STARTUP );
      }

      inline bool inSubState( uint8_t _state )
      {
         return ( state.sub == _state );
      }

      inline bool isReady()
      {
         return ( ( state.main == IDLE ) || ( state.main == FAILTURE ) );
      }

      void setRemoteAccess( bool enable )
      {
         remoteAccess = enable;
      }

      bool hasRemoteAccess()
      {
         return remoteAccess;
      }

      void terminate();

   protected:

      void setMainState( uint8_t mainState );

      void setState( const State& p_state );

      void setSubState( uint8_t subState );

      void setGlobalSleepDelay( uint16_t delay );

      ////    Additional operations    ////

   public:

      inline uint16_t getSleepTime() const
      {
         return sleepTime;
      }

      inline void setSleepTime( uint16_t p_sleepTime )
      {
         sleepTime = p_sleepTime;
      }

      ////    Attributes    ////

      uint16_t sleepTime;

   protected:

      State state;

      bool remoteAccess;
};

#endif
