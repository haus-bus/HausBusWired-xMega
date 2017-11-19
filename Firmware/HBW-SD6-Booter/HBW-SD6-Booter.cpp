/*
 * HBW-SD6-Booter.cpp
 *
 * Created: 30.09.2017 02:31:44
 *  Author: Viktor Pankraz
 */

#include "HBW-SD6-Booter.h"

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
   "$MOD$ HmwBooter",
   0,
   Release::MAJOR,
   Release::MINOR,
   Release::HMW_SD6_ID,
   0
};

static const uint8_t debugLevel( DEBUG_LEVEL_LOW );

#define getId() FSTR( "BOOTER" )

static bool isFirmwareValid = false;
static bool startFirmware = false;

#ifdef DEBUG
void putChar( char c )
{
   Usart::instance<PortC, 1>().write( c );
}
#endif

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

static const uint8_t LED_MASK = 0x3F;
static uint8_t ledData[] = { 0x00, 0x01, 0x05, 0x15, 0x35, 0x3D, 0x3F, 0x3E, 0x3A, 0x2A, 0x0A, 0x02 };
static uint8_t ledIdx = 0;
static bool isDownloadRunning = false;

void notifyNextDownloadPacket()
{
   if ( ++ledIdx >= sizeof( ledData ) )
   {
      ledIdx = 0;
   }
}

void handleLeds()
{
   static Timestamp lastTime;
   if ( isDownloadRunning )
   {
      uint8_t otherPins = PORTC.OUT & ~LED_MASK;
      PORTC.OUT = ledData[ledIdx] | otherPins;
   }
   else
   {
      if ( PORTC.IN & LED_MASK )
      {
         if ( lastTime.since() > 990 )
         {
            PORTC.OUTTGL = LED_MASK;
            lastTime = Timestamp();
         }
      }
      else
      {
         if ( lastTime.since() > 10 )
         {
            PORTC.OUTTGL = LED_MASK;
            lastTime = Timestamp();
         }
      }
   }
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

   // Initialize interfaces
   // DigitalOutput rxEnable( PortE, 0 ), txEnable( PortE, 1 );
   // DigitalInput rx( PortE, 2 );
   // DigitalOutput tx( PortE, 3 );
   PORTE.DIR = Pin0 | Pin1 | Pin3;
   PORTC.DIR = LED_MASK;

#ifdef DEBUG
   DigitalInput rx( PortC, 6 );
   DigitalOutput tx( PortC, 7 );
   Usart::instance<PortC, 1>().init<115200>();
   Logger::instance().setStream( putChar );
#endif

   // Authorize interrupts
   // InterruptController::selectBootInterruptSection();
   // InterruptController::enableAllInterruptLevel();
   // GlobalInterrupt::enable();
   HmwDevice::deviceType = Release::HMW_SD6_ID;
   HmwDevice::firmwareVersion = ( ( Release::MAJOR << 8 ) | Release::MINOR );
   HmwDevice::hardwareVersion = Release::REV_0;
   HmwDevice::basicConfig = reinterpret_cast<HmwDevice::BasicConfig*>( MAPPED_EEPROM_START );
   HmwDevice::ownAddress = changeEndianness( HmwDevice::basicConfig->ownAdress );

   HmwStream::setStream( Usart::instance<PortE, 0>() );

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
         notifyNextDownloadPacket();
      }
      HmwDevice::processMessage( msg );
      HmwDevice::handleAnnouncement();
      handleLeds();
      WatchDog::reset();
   }
   return 0;
}