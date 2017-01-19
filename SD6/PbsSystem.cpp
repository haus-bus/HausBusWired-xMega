/*
 * MS6System.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "DebugOptions.h"
#include <Gateway.h>
#include "HomeAutomation.h"
#include <PersistentRules.h>
#include <RuleEngine.h>
#include <Scheduler.h>
#include <Time/WeekTime.h>
#include <Release.h>
#include "PbsSystem.h"

const ModuleId moduleId = { "$MOD$ SD6      ", 0, Release::MAJOR,
Release::MINOR, Release::SD6_ID, 0 };

PbsSystem::PbsSystem() :
    digitalPortA( PortA ), digitalPortB( PortB ), digitalPortC( PortC ),
    digitalPortD( PortD )
{
  digitalPortB.setNotUseablePins( Pin4 | Pin5 | Pin6 | Pin7 );
  if ( CONTROLLER_ID == Release::MS6_ID )
  {
    digitalPortC.setNotUseablePins( Pin6 | Pin7 );
  }
}

void PbsSystem::start()
{
  static const uint8_t MAX_JOBS = 40;
  Scheduler::setup( MAX_JOBS );

  SystemTime::init( SystemTime::RTCSRC_RCOSC_1024, 1024 );

  static PbsSystem ms6;
  Scheduler::runJobs();
}
