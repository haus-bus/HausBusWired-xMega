/*
 * AR8System.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include "AR8System.h"
#include "DebugOptions.h"
#include <Gateway.h>
#include "HomeAutomation.h"
#include <PersistentRules.h>
#include <RuleEngine.h>
#include <Scheduler.h>
#include <Time/WeekTime.h>

AR8System::AR8System() :
   digitalPortE( PortE ), digitalPortF( PortF )
{
   if ( getFckE() < FCKE_V4_0 )
   {
      digitalPortE.setNotUseablePins( Pin0 | Pin1 | Pin4 | Pin5 );
   }
   else
   {
      digitalPortE.setNotUseablePins( Pin0 | Pin1 | Pin2 | Pin3 );
   }

#ifdef _TRACE_PORT_
   digitalPortF.terminate();
#endif
}

void AR8System::start()
{
   static const uint8_t MAX_JOBS = 64;
   static const uint8_t MAX_EVENTS = 64;
   Scheduler::setup( MAX_JOBS, MAX_EVENTS );

   SystemTime::init( SystemTime::RTCSRC_RCOSC_1024, 1024 );

   static AR8System ar8;
   Scheduler::runJobs();
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
