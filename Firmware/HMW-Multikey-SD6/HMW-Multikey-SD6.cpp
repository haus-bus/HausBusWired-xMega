/*
 * HBW-SD6-Multikey.cpp
 *
 * Created: 03.12.2016 02:31:44
 *  Author: Viktor Pankraz
 */

#include "HmwMultiKeyFactory.h"

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
   Release::HMW_SD6,
   0
};

int main( void )
{
   SystemTime::init();
   Eeprom::MemoryMapped::enable();
   HmwDevice::setHardware( Release::HMW_SD6, HmwMultiKeyFactory::createHardware() );

   // Authorize interrupts
   InterruptController::selectAppInterruptSection();
   InterruptController::enableAllInterruptLevel();
   GlobalInterrupt::enable();

   while ( 1 )
   {
      HmwDevice::loop();
   }
}