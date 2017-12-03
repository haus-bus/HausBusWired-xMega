/*
 * HBW-SD6-Booter.cpp
 *
 * Created: 30.09.2017 02:31:44
 *  Author: Viktor Pankraz
 */

#include "HBW-SD6-Booter.h"
#include "HmwBooterHw.h"

#include <Peripherals/InterruptController.h>
#include <Peripherals/WatchDog.h>
#include <Peripherals/Flash.h>
#include <Peripherals/ResetSystem.h>
#include <DigitalOutput.h>
#include <Release.h>

#include <Time/SystemTime.h>
#include <HBWired/HmwStream.h>


const ModuleId moduleId =
{
   "$MOD$ " MOD_ID,
   BOOTER_SIZE,
   Release::MAJOR,
   Release::MINOR,
   Release::CONTROLLER_ID,
   0
};

static const uint8_t debugLevel( DEBUG_LEVEL_LOW );

#define getId() FSTR( "BOOTER" )

static bool isDownloadRunning = false;
static bool isFirmwareValid = false;
static bool startFirmware = false;

static HmwBooterHw hardware;

void checkFirmware()
{
   DEBUG_H1( FSTR( ".checkFirmware()" ) );

   ModuleId installedMod;

   if ( Flash::read( _VECTORS_SIZE, &installedMod, sizeof( ModuleId ) ) != sizeof( ModuleId ) )
   {
      ERROR_1( FSTR( "Flash::read() failed" ) );
      return;
   }

   if ( moduleId.getFirmwareId() == installedMod.getFirmwareId() )
   {
      uint32_t fCrc;
      uint32_t cCrc = Flash::getRangeCRC( 0, installedMod.getSize() - 1 );
      Flash::read( installedMod.getSize(), &fCrc, sizeof( fCrc ) );

      DEBUG_M2( FSTR( "name:    " ), installedMod.name );
      DEBUG_M2( FSTR( "size:    0x" ), installedMod.size );
      DEBUG_M4( FSTR( "release: " ), installedMod.majorRelease, '.', installedMod.minorRelease );
      DEBUG_M2( FSTR( "cCRC:    0x" ), cCrc );
      DEBUG_M2( FSTR( "fCRC:    0x" ), fCrc );

      if ( ( fCrc == cCrc ) && ( Release::MAJOR == installedMod.majorRelease ) )
      {
         isFirmwareValid = true;
         startFirmware = true;
         return;
      }
   }
   ERROR_1( FSTR( "invalid" ) );
}

void startApplication()
{
     #ifdef EIND
   EIND = 0;
     #endif

   void ( *start )( void ) = NULL;
   start();
}

int main( void )
{
   SystemTime::init();
   Eeprom::MemoryMapped::enable();

   HmwStream::setHardware( &hardware );
   HmwDevice::setup( Release::HMW_SD6_ID, reinterpret_cast<HmwDevice::BasicConfig*>( MAPPED_EEPROM_START ) );

   checkFirmware();

   while ( 1 )
   {
      if ( startFirmware && !ResetSystem::isSoftwareReset() )
      {
         startApplication();
      }
      if ( HmwDevice::isReadConfigPending() )
      {
         HmwDevice::clearPendingReadConfig();
         ResetSystem::clearSources();
         checkFirmware();
      }
      HmwMessageBase* msg = HmwStream::pollMessageReceived();
      if ( msg && msg->isCommand( HmwMessageBase::WRITE_FLASH ) )
      {
         isDownloadRunning = true;
         startFirmware = false;
         hardware.notifyNextDownloadPacket();
      }
      HmwDevice::processMessage( msg );
      HmwDevice::handleAnnouncement();
      hardware.handleLeds( isDownloadRunning );
      WatchDog::reset();
   }
   return 0;
}