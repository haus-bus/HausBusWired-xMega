/*
 * HomeAutomationHw.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "HomeAutomationHw.h"
#include <Security/Checksum.h>
#include <FlashString.h>
#include <Traces/Logger.h>
#include <Security/ModuleId.h>
#include <Peripherals/Oscillator.h>
#include <Peripherals/Clock.h>
#include <Peripherals/Eeprom.h>
#include <Peripherals/RealTimeCounter.h>
#include <Peripherals/DigitalFrequencyLockedLoops.h>

const uint8_t HomeAutomationHw::debugLevel( DEBUG_LEVEL_OFF );


#ifdef _DEBUG_

FlashString* HomeAutomationHw::getId()
{
   return FSTR( "SystemHw::" );
}

#endif


bool HomeAutomationHw::getModuleId( uint8_t index, ModuleId* moduleId )
{
   uint32_t modulIdPosition = findModuleIdPosition( index );
   DEBUG_H2( FSTR( "ModIdPos:" ), modulIdPosition );

   if ( modulIdPosition == 0xFFFFFFFF )
   {
      return false;
   }

   if ( Flash::read( modulIdPosition, moduleId, sizeof( ModuleId ) )
        == sizeof( ModuleId ) )
   {
      return ( getFirmwareId() == moduleId->getFirmwareId() );
   }
   return false;
}

static void
__attribute__( ( section( ".init3" ), naked, used ) )
lowLevelInit( void )
{
#ifdef EIND
   __asm volatile ( "ldi r24,pm_hh8(__trampolines_start)\n\t"
                    "out %i0,r24" ::"n" ( &EIND ) : "r24", "memory" );
#endif
#ifdef _DEBUG_
   WatchDog::disable();
#else
   WatchDog::enable( WatchDog::_4S );
#endif
   InterruptController::selectAppInterruptSection();

#if F_CPU == 8000000
   Clock::configPrescalers( CLK_PSADIV_4_gc, CLK_PSBCDIV_1_1_gc );
#else
   Clock::configPrescalers( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
#endif

   // Enable internal 32 MHz and 32kHz ring oscillator and wait until they are stable.
   Oscillator::enable( OSC_RC32MEN_bm | OSC_RC32KEN_bm );
   Oscillator::waitUntilOscillatorIsReady( OSC_RC32MEN_bm | OSC_RC32KEN_bm );

   // Set the 32 MHz ring oscillator as the main clock source.
   Clock::selectMainClockSource( CLK_SCLKSEL_RC32M_gc );

   Clock::enableRTCClockSource( CLK_RTCSRC_RCOSC_gc );
   RealTimeCounter::init( 0xFFFF, 0, 0, RTC_PRESCALER_DIV1_gc );

   DigitalFrequencyLockedLoops::instance( true ).enableAutoCalibration();

   Eeprom::MemoryMapped::enable();
}