/*
 * MS6System.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "HomeAutomation.h"
#include "DebugOptions.h"
#include "PbsSystem.h"

#include <Gateway.h>
#include <PersistentRules.h>
#include <RuleEngine.h>
#include <Scheduler.h>
#include <Time/WeekTime.h>
#include <Release.h>
#include <PortPinUnit.h>
#include <DS1820.h>

PbsSystem::PbsSystem()
{
   DigitalPort* digitalPortA = new DigitalPort( PortA );
   DigitalPort* digitalPortC = new DigitalPort( PortC );

   if ( hardware.getFirmwareId() == SD485_ID )
   {
      uint8_t fcke = getFckE();
      switch ( fcke )
      {
         case SD485_LC4_0V:
         case SD485_LC4_1V:
         {
            digitalPortA->setNotUseablePins( Pin4 | Pin5 | Pin6 | Pin7 );
            digitalPortC->setNotUseablePins( Pin4 | Pin5 | Pin6 | Pin7 );
            new PortPinUnit( PortPin( PortR, 1 ) );   // red led
            break;
         }
         case SD485_IO12:
         {
            new PortPinUnit( PortPin( PortA, 6 ) );   // red led
         }
         default:
         {
            DS1820::scanAndCreateDevices( PortPin( PortR, 1 ) );
            DigitalPort* digitalPortB = new DigitalPort( PortB );
            DigitalPort* digitalPortD = new DigitalPort( PortD );
            digitalPortA->setNotUseablePins( Pin6 | Pin7 );
            digitalPortB->setNotUseablePins( Pin4 | Pin5 | Pin6 | Pin7 );
            digitalPortD->setNotUseablePins( Pin6 | Pin7 );
         }
      }
   }
   else // SD6
   {
      DigitalPort* digitalPortB = new DigitalPort( PortB );
      DigitalPort* digitalPortD = new DigitalPort( PortD );
      digitalPortA->setNotUseablePins( Pin6 | Pin7 );
      digitalPortB->setNotUseablePins( Pin4 | Pin5 | Pin6 | Pin7 );
      digitalPortD->setNotUseablePins( Pin6 | Pin7 );
   }
   hardware.configureLogicalButtons();
}

void PbsSystem::start()
{
   static const uint8_t MAX_JOBS = 40;
   Scheduler::setup( MAX_JOBS );

   SystemTime::init( SystemTime::RTCSRC_RCOSC_1024, 1024 );

   static PbsSystem ms6;
   Scheduler::runJobs();
}
