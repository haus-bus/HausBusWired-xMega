/*
 * HBW-SD6-Booter.cpp
 *
 * Created: 30.09.2017 02:31:44
 *  Author: Viktor Pankraz
 */

#include "HBW-SD6-Booter.h"

#include <Peripherals/InterruptController.h>
#include <Peripherals/WatchDog.h>
#include <Peripherals/Oscillator.h>
#include <Peripherals/Clock.h>
#include <Peripherals/Flash.h>
#include <Peripherals/ResetSystem.h>
#include <DigitalOutput.h>
#include <Release.h>

#include <Time/SystemTime.h>
#include <HBWired/HmwMessage.h>
#include <Elements/SerialStream.h>


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

static Timestamp lastReceivedTime( 0 );

static bool isFirmwareValid = false;
static bool startFirmware = false;

#ifdef DEBUG
void putChar( char c )
{
   Usart::instance<PortC, 1>().write( c );
}
#endif

bool isBusIdle()
{
   return lastReceivedTime.since() > ( 100 + ( HmwDevice::ownAddress& 0x7F ) );
}

#define getRs485Stream() Usart::instance<PortE, 0>()


Stream::Status sendMessage( HmwMessage& msg )
{
   // static DigitalOutput txEnable( PortE, 1 );

   uint8_t data;
   Stream::Status status = Stream::SUCCESS;

   msg.prepareToSend();

   // txEnable.set()
   PORTE.OUTSET = Pin1;
   while ( msg.getNextByteToSend( data ) )
   {
      if ( !getRs485Stream().write( data ) )
      {
         status = Stream::ABORTED;
      }
   }
   getRs485Stream().waitUntilTransferCompleted();
   // txEnable.clear();
   PORTE.OUTCLR = Pin1;
   return status;
}

void handleAnnouncement()
{
   static bool announced = false;
   if ( !announced && ( SystemTime::now() > 1500 ) && isBusIdle() )
   {
      HmwMessage msg;
      msg.setupAnnounce( 0 );
      announced = sendMessage( msg ) == Stream::SUCCESS;
   }
}

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

   getRs485Stream().init<19200, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc, USART_CHSIZE_8BIT_gc, true, false>();
   PORTC.DIR = LED_MASK;

#ifdef DEBUG
   DigitalInput rx( PortC, 6 );
   DigitalOutput tx( PortC, 7 );
   Usart::instance<PortC, 1>().init<115200>();
   Logger::instance().setStream( putChar );
#endif

   // Authorize interrupts
   // InterruptController::enableAllInterruptLevel();
   // GlobalInterrupt::enable();
   HmwDevice::deviceType = Release::HMW_SD6_ID;
   HmwDevice::firmwareVersion = ( ( Release::MAJOR << 8 ) | Release::MINOR );
   HmwDevice::hardwareVersion = Release::REV_0;
   HmwDevice::basicConfig = reinterpret_cast<HmwDevice::BasicConfig*>( MAPPED_EEPROM_START );
   HmwDevice::ownAddress = changeEndianness( HmwDevice::basicConfig->ownAdress );

   checkFirmware();

   HmwMessage inMessage;

   // special case: if FW starts the booter with UPDATE command, send ANNONCE by setting the system time to 1500ms
   if ( ResetSystem::isSoftwareReset() )
   {
      SystemTime::set( 1500 );
   }


   uint8_t data;
   while ( 1 )
   {
      if ( getRs485Stream().isReceiveCompleted() )
      {
         lastReceivedTime = Timestamp();
         if ( getRs485Stream().read( data ) )
         {
            if ( inMessage.nextByteReceived( data ) )
            {
               if ( inMessage.isForMe() && !inMessage.isDiscovery() )
               {
                  if ( inMessage.isCommand( HmwMessage::WRITE_FLASH ) )
                  {
                     isDownloadRunning = true;
                     startFirmware = false;
                     notifyNextDownloadPacket();
                  }
                  if ( inMessage.isCommand( HmwMessage::READ_CONFIG ) )
                  {
                     ResetSystem::clearSources();
                     checkFirmware();
                  }
                  if ( inMessage.generateResponse() )
                  {
                     sendMessage( inMessage );
                  }
               }
            }
         }
      }
      handleAnnouncement();
      handleLeds();
      WatchDog::reset();
      if ( startFirmware && !ResetSystem::isSoftwareReset() )
      {
         startApplication();
      }
   }
   return 0;
}


static void
__attribute__( ( section( ".init3" ), naked, used ) )
lowLevelInit( void )
{
#ifdef EIND
   __asm volatile ( "ldi r24,pm_hh8(__trampolines_start)\n\t"
                    "out %i0,r24" ::"n" ( &EIND ) : "r24", "memory" );
#endif

#ifdef DEBUG
   WatchDog::disable();
 #else
   WatchDog::enable( WatchDog::_4S );
   InterruptController::selectBootInterruptSection();
#endif


   Clock::configPrescalers( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );

   // Enable internal 32 MHz and 32kHz ring oscillator and wait until they are stable.
   Oscillator::enable( OSC_RC32MEN_bm | OSC_RC32KEN_bm );
   Oscillator::waitUntilOscillatorIsReady( OSC_RC32MEN_bm | OSC_RC32KEN_bm );

   // Set the 32 MHz ring oscillator as the main clock source.
   Clock::selectMainClockSource( CLK_SCLKSEL_RC32M_gc );
}