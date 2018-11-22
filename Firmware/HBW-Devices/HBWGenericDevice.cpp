/*
 * HBWGenericDevice.cpp
 *
 * Created: 03.12.2016 02:31:44
 *  Author: Viktor Pankraz
 */

#include "HBWDeviceFactory.h"

#include <Release.h>
#include <Time/SystemTime.h>
#include <Peripherals/Flash.h>
#include <Peripherals/InterruptController.h>
#include <DigitalOutput.h>

#include <Security/ModuleId.h>

extern __attribute__( ( section( ".vectors" ) ) ) const uint16_t moduleIdCCU;
extern __attribute__( ( section( ".vectors" ) ) ) const ModuleId moduleId;


const uint16_t moduleIdCCU = ( Release::MAJOR << 8 ) | Release::MINOR;

const ModuleId moduleId =
{
   "$MOD$ HBW-DEV  ",
   0,
   Release::MAJOR,
   Release::MINOR,
   Release::HBW_GENERIC,
   0
};

int main( void )
{
   SystemTime::init();
   Eeprom::MemoryMapped::enable();

   // The Booter module id specifies the device hardware, so get the id to create the correct device
   ModuleId booterModId;
   if ( NvmController::readUserSignature( 0, &booterModId, sizeof( ModuleId ) ) != sizeof( ModuleId ) )
   {
      ERROR_1( FSTR( "Flash::read() failed" ) );
      booterModId.firmwareId = Release::HBW_GENERIC;
   }
   HmwDeviceHw* hardware = HBWDeviceFactory::createDevice( (Release::FirmwareId)booterModId.firmwareId );
   HmwDevice::setHardware( booterModId.firmwareId, hardware );

   // initialize random generator with unique device id
   srand( hardware->getBasicConfig()->ownAddress );

   // Authorize interrupts
   InterruptController::selectAppInterruptSection();
   InterruptController::enableAllInterruptLevel();
   GlobalInterrupt::enable();

   while ( 1 )
   {
      HmwDevice::loop();
   }
}