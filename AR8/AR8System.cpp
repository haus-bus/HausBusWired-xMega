/********************************************************************
   Rhapsody	: 8.0.3
   Login		: viktor.pankraz
   Component	: AR8
   Configuration        : release
   Model Element	: AR8System
   //!	Generated Date	: Wed, 6, Aug 2014
   File Path	: AR8/release/Systems/HomeAutomationSystems/AR8System.cpp
 *********************************************************************/

// ## auto_generated
#include "AR8System.h"
// ## dependency DebugOptions
#include "DebugOptions.h"
// ## dependency Gateway
#include <Gateway.h>
// ## dependency MyEvent
#include "HomeAutomation.h"
// ## dependency PersistentRules
#include <PersistentRules.h>
// ## dependency RuleEngine
#include <RuleEngine.h>
// ## dependency Scheduler
#include <Scheduler.h>
// ## dependency WeekTime
#include <Time/WeekTime.h>
// ## package Systems::HomeAutomationSystems

// ## class AR8System
AR8System::AR8System() :
   digitalPortE( PortE ), digitalPortF( PortF )
{
   // #[ operation AR8System()
   digitalPortE.setNotUseablePins( Pin0 | Pin1 | Pin4 | Pin5 );

#ifdef _TRACE_PORT_
   digitalPortF.terminate();
#endif
   // #]
}

void AR8System::start()
{
   // #[ operation start()
   static const uint8_t MAX_JOBS = 64;
   static const uint8_t MAX_EVENTS = 64;
   Scheduler::setup( MAX_JOBS, MAX_EVENTS );

   SystemTime::init( SystemTime::RTCSRC_RCOSC_1024, 1024 );

   static AR8System ar8;
   Scheduler::runJobs();
   // #]
}

DigitalPort* AR8System::getDigitalPortE() const
{
   return (DigitalPort*) &digitalPortE;
}

DigitalPort* AR8System::getDigitalPortF() const
{
   return (DigitalPort*) &digitalPortF;
}

AR8SystemHw* AR8System::getHardware() const
{
   return (AR8SystemHw*) &hardware;
}

/*********************************************************************
   File Path	: AR8/release/Systems/HomeAutomationSystems/AR8System.cpp
*********************************************************************/
