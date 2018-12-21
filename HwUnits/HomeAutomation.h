/*
 * HomeAutomation.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Systems_HomeAutomation_H
#define Systems_HomeAutomation_H

#include "HomeAutomationInterface.h"
#include "HomeAutomationConfiguration.h"

#include <HomeAutomationHw.h>
#include <Reactive.h>
#include <SystemTime.h>
#include <IResponse.h>

#include <Time/Calender.h>

class CriticalSection;

class DebugOptions;

class Event;

class Gateway;

class HACF;

class PersistentRules;

class RuleEngine;

class Scheduler;

class WeekTime;

class evMessage;

class HomeAutomation : public Reactive
{
   public:

      class LogicalUnitGroup;

      class MyEvent;

      static const uint16_t TIME_TO_RESET = 100;
      static const uint8_t MAX_TIME_DIFFERENCE = 8;

      class LogicalUnitGroup
      {
         public:

            static const uint8_t MAX_GROUPS = 16;

            ////    Operations    ////

            static uint8_t setState( const HomeAutomationInterface::Command::SetUnitGroupState& params );

            ////    Attributes    ////

            static uint16_t state[MAX_GROUPS];
      };

      class MyEvent : public IResponse
      {
         public:

            enum Events
            {
               EVENT_TIME = HACF::EVENTS_START,
               EVENT_NEW_DEVICE_ID,
               EVENT_STARTED,
               EVENT_GROUP_ON,
               EVENT_GROUP_UNDEFINED,
               EVENT_GROUP_OFF,
               EVENT_DAY,
               EVENT_NIGHT
            };

            ////    Constructors and destructors    ////

            inline MyEvent() :
               IResponse( HACF::SYSTEM_ID )
            {
               controlFrame.receiverId.setId( ( (uint32_t) HACF::deviceId << 16 ) | HACF::SYSTEM_ID );
               controlFrame.setDataLength( 1 );
            }

            ////    Operations    ////

            void setTimeEvent();
      };

      ////    Constructors and destructors    ////

      inline HomeAutomation() :
         errorEvent( HACF::SYSTEM_ID )
      {
         setId( ( ClassId::SYSTEM << 8 ) | HACF::SYSTEM_ID );
         HomeAutomationConfiguration& conf = HomeAutomationConfiguration::instance();
         HACF::deviceId = conf.getDeviceId();
         Calender::minuteListener = this;
         // give some additional time to get e.g. a valid IP address from DHCP Server
         setGlobalSleepDelay( conf.getStartupDelay() << 3 );
      }

      ////    Operations    ////

      virtual bool notifyEvent( const Event& event );

      void run();

   protected:

      void checkPersistentRules();

      void cmdGetRemoteObjects( HomeAutomationInterface::Response& response );

      void cmdReadMemory( HomeAutomationInterface::Command::ReadMemory& parameter,
                          HomeAutomationInterface::Response& response );

      void cmdWriteRules( HomeAutomationInterface::Command::WriteRules& parameter,
                          uint16_t dataLength,
                          HomeAutomationInterface::Response& response );

      bool handleRequest( HACF* message );

   private:

      uint16_t getMinuteTicks() const;

      ////    Additional operations    ////

   public:

      HomeAutomationInterface::Response* getErrorEvent() const;

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

   private:

      ////    Attributes    ////

   protected:

      static const uint8_t debugLevel;

   private:

      static Timestamp lastMemoryReportTime;

      ////    Relations and components    ////

   protected:

      HomeAutomationInterface::Response errorEvent;

};


#endif
