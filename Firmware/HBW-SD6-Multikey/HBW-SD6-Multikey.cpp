/*
 * HBW-SD6-Multikey.cpp
 *
 * Created: 03.12.2016 02:31:44
 *  Author: Viktor Pankraz
 */

#include "MultiKeyHw.h"

#include <Release.h>
#include <Time/SystemTime.h>
#include <Peripherals/InterruptController.h>
#include <DigitalOutput.h>

#include <Security/ModuleId.h>

extern __attribute__( ( section( ".vectors" ) ) ) const ModuleId moduleId;

const ModuleId moduleId =
{
   "$MOD$ HBW-SD6  ",
   0,
   Release::MAJOR,
   Release::MINOR,
   Release::HMW_SD6_ID,
   0
};

int main( void )
{
   SystemTime::init();
   Eeprom::MemoryMapped::enable();

   HmwStream::setHardware( MultiKeyHw::create() );
   HmwDevice::setup( Release::HMW_SD6_ID, MultiKeyHw::getBasicConfig() );

   // Authorize interrupts
   InterruptController::selectAppInterruptSection();
   InterruptController::enableAllInterruptLevel();
   GlobalInterrupt::enable();

   while ( 1 )
   {
      HmwDevice::loop();
   }
}