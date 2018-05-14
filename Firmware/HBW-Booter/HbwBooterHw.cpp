/*
 * HbwBooterHw.cpp
 *
 * Created: 23.11.2017 19:23:55
 * Author: viktor.pankraz
 */


#include "HBWBooterHw.h"
#include <Tracing/Logger.h>
#include <Time/Timestamp.h>
#include <DigitalOutput.h>
#include <Release.h>

#if ( DEVICE_ID == 0xB1 )  // Release::HBW_MultiKeySD6

#define LED_PORT PORTC
#define LED_MASK 0x3F
static uint8_t ledData[] = { 0x00, 0x01, 0x05, 0x15, 0x35, 0x3D, 0x3F, 0x3E, 0x3A, 0x2A, 0x0A, 0x02 };

static DigitalInputTmpl<PortE, 2> rx;
static DigitalOutputTmpl<PortE, 3> tx;

#if ( HARDWARE_ID == 0 )
static DigitalOutputTmpl<PortE, 0> rxEnable;
static DigitalOutputTmpl<PortE, 1> txEnable;
#elif ( HARDWARE_ID == 1 )
static DigitalOutputTmpl<PortR, 0> txEnable;
#else
#error "Hardware revision not supported!!!"
#endif

#elif ( DEVICE_ID == 0xB2 )  // Release::HBW_IO_12_1W_UP

#define LED_PORT PORTA
#define LED_MASK 0x40
static DigitalOutputTmpl<PortR, 0> txEnable;
static DigitalInputTmpl<PortE, 2> rx;
static DigitalOutputTmpl<PortE, 3> tx;

#if ( HARDWARE_ID == 0 )
static uint8_t ledData[] = { 0x00 };
#elif ( HARDWARE_ID == 1 )
static uint8_t ledData[] = { 0x00, LED_MASK };
#else
#error "Hardware revision not supported!!!"
#endif

#elif ( DEVICE_ID == 0xB3 )  // Release::HBW_4LC_4IN_DR

#define LED_PORT PORTR
#define LED_MASK 0x02

#if ( HARDWARE_ID == 0 )
static DigitalOutputTmpl<PortR, 0> txEnable;
static DigitalInputTmpl<PortE, 2> rx;
static DigitalOutputTmpl<PortE, 3> tx;
static uint8_t ledData[] = { 0x00, LED_MASK };
#else
#error "Hardware revision not supported!!!"
#endif

#else
#error "HBW-Device not supported!!!"
#endif

static uint8_t ledIdx = 0;
static Timestamp lastTime;

HBWBooterHw::HBWBooterHw()
{
   serial = &Usart::instance<PortE, 0>();
   LED_PORT.DIRSET = LED_MASK;

#ifdef DEBUG
   DigitalInputTmpl< PortD, 6 > rx;
   DigitalOutputTmpl<PortD, 7> tx;
   Usart::instance<PortD, 1>().init<115200>();
   Logger::instance().setStream( HBWBooterHw::debug );
#endif
}

void HBWBooterHw::enableTranceiver( bool enable )
{
   enable ? txEnable.set() : txEnable.clear();
}

void HBWBooterHw::debug( char c )
{
   Usart::instance<PortD, 1>().write( c );
}

void HBWBooterHw::notifyNextDownloadPacket()
{
   if ( ++ledIdx >= sizeof( ledData ) )
   {
      ledIdx = 0;
   }
}



void HBWBooterHw::handleLeds( bool isDownloadRunning )
{
   if ( isDownloadRunning )
   {
      uint8_t otherPins = LED_PORT.OUT & ~LED_MASK;
      LED_PORT.OUT = ledData[ledIdx] | otherPins;
   }
   else
   {
      if ( lastTime.since() > 500 )
      {
         LED_PORT.OUTTGL = LED_MASK;
         lastTime = Timestamp();
      }
   }
}

#include <Peripherals/Oscillator.h>
#include <Peripherals/Clock.h>
#include <Peripherals/DigitalFrequencyLockedLoops.h>
#include <Peripherals/WatchDog.h>

static void
__attribute__( ( section( ".reset" ), naked, used ) )
ResetVector( void )
{
   asm ( "jmp __ctors_end" );
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
   #endif


   Clock::configPrescalers( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );

   // Enable internal 32 MHz and 32kHz ring oscillator and wait until they are stable.
   Oscillator::enable( OSC_RC32MEN_bm | OSC_RC32KEN_bm );
   Oscillator::waitUntilOscillatorIsReady( OSC_RC32MEN_bm | OSC_RC32KEN_bm );

   // Set the 32 MHz ring oscillator as the main clock source.
   Clock::selectMainClockSource( CLK_SCLKSEL_RC32M_gc );

   // save some power and disable the 2MHz oscillator
   Oscillator::disable( OSC_RC2MEN_bm );

   DigitalFrequencyLockedLoops::instance( true ).enableAutoCalibration();
}