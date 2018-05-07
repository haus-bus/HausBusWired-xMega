/*
 * HBWGenericDeviceHw.cpp
 *
 * Created: 20.01.2018 11:52:48
 *  Author: viktor.pankraz
 */

 #include "HBWGenericDeviceHw.h"

 #include <Peripherals/Oscillator.h>
 #include <Peripherals/Clock.h>
 #include <Peripherals/DigitalFrequencyLockedLoops.h>
 #include <Peripherals/WatchDog.h>
 #include <DigitalOutput.h>

void debug( char c )
{
   Usart::instance<PortD, 1>().write( c );
}

HBWGenericDeviceHw::HBWGenericDeviceHw()
{
   // setup debug console
        #ifdef DEBUG
   DigitalInputTmpl< PortD, 6 > rxDebug;
   DigitalOutputTmpl<PortD, 7> txDebug;
   Usart::instance<PortD, 1>().init<115200>();
   Logger::instance().setStream( debug );
        #endif

   // setup the serial for HmwStream, the txEnable/rxEnable lines have to be set in the special HW version
   DigitalInputTmpl< PortE, 2 > rxHmwStream;
   DigitalOutputTmpl<PortE, 3> txHmwStream;
   serial = &Usart::instance<PortE, 0>();
}

SIGNAL(USARTE0_RXC_vect)
{
   HmwStream::nextByteReceivedFromISR(Usart::instance<PortE,0>().readDataRegisterFromISR());
}

static void
__attribute__(( section(".init3"),naked,used ))
lowLevelInit(void)
{
   #ifdef EIND
   __asm volatile ( "ldi r24,pm_hh8(__trampolines_start)\n\t"
                    "out %i0,r24" ::"n" ( &EIND ) : "r24","memory" );
   #endif

   #ifdef DEBUG
   WatchDog::disable();
   #else
   WatchDog::enable(WatchDog::_4S);
   #endif


   Clock::configPrescalers(CLK_PSADIV_1_gc,CLK_PSBCDIV_1_1_gc);

   // Enable internal 32 MHz and 32kHz ring oscillator and wait until they are stable.
   Oscillator::enable(OSC_RC32MEN_bm | OSC_RC32KEN_bm);
   Oscillator::waitUntilOscillatorIsReady(OSC_RC32MEN_bm | OSC_RC32KEN_bm);

   // Set the 32 MHz ring oscillator as the main clock source.
   Clock::selectMainClockSource(CLK_SCLKSEL_RC32M_gc);

   // save some power and disable the 2MHz oscillator
   Oscillator::disable(OSC_RC2MEN_bm);

   DigitalFrequencyLockedLoops::instance(true).enableAutoCalibration();
}