/*
 * Peripherals.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */


#ifndef Peripherals_Peripherals_H
#define Peripherals_Peripherals_H

#include <avr/io.h>
#include <CriticalSection.h>
#include <Tracing/Logger.h>

class Adc;

class Clock;

class DigitalFrequencyLockedLoops;

class DmaChannel;

class DmaController;

class Eeprom;

class EventSystem;

class Flash;

class InterruptController;

class IoPort;

class NvmController;

class Oscillator;

class RealTimeCounter;

class ResetSystem;

class Spi;

class TimerCounter;

class TimerCounterBase;

class Twi;

class Usart;

class WatchDog;

enum Pin
{
   NoPin = 0x00,
   Pin0 = 0x01,
   Pin1 = 0x02,
   Pin2 = 0x04,
   Pin3 = 0x08,
   Pin4 = 0x10,
   Pin5 = 0x20,
   Pin6 = 0x40,
   Pin7 = 0x80,
   AllPins = 0xFF
};

enum Port
{
   Port0 = 0,
   Port1,
   Port2,
   Port3,
   Port4,
   Port5,
   Port6,
   Port7,
   Port8,
   Port9,
   Port10,
   Port11,
   Port12,
   Port13,

   PortA = 0,
   PortB,
   PortC,
   PortD,
   PortE,
   PortF,
   PortG,
   PortH,
   PortI,
   PortJ,
   PortK,
   PortQ,
   PortR,
   PortDummy,
   PortMax,
};

#define TRACE_PORT 0x80

#ifdef _TRACE_PORT_
  #define TRACE_PORT_INIT( pins )     _TRACE_PORT_.DIRSET = pins;    _TRACE_PORT_.OUTCLR = pins;
  #define TRACE_PORT_PUT( pins )      if ( debugLevel & TRACE_PORT ) { _TRACE_PORT_.OUT = pins; }
  #define TRACE_PORT_SET( pins )      if ( debugLevel & TRACE_PORT ) { _TRACE_PORT_.OUTSET = pins; }
  #define TRACE_PORT_CLEAR( pins )    if ( debugLevel & TRACE_PORT ) { _TRACE_PORT_.OUTCLR = pins; }
  #define TRACE_PORT_TOGGLE( pins )   if ( debugLevel & TRACE_PORT ) { _TRACE_PORT_.OUTTGL = pins; }
#else
  #define TRACE_PORT_INIT( pins )
  #define TRACE_PORT_PUT( pins )
  #define TRACE_PORT_SET( pins )
  #define TRACE_PORT_CLEAR( pins )
  #define TRACE_PORT_TOGGLE( pins )
#endif

///*! \brief CCP write helper function written in assembly.
// *
// *  This function is written in assembly because of the timecritial
// *  operation of writing to the registers.
// *
// *  \param address A pointer to the address to write to.
// *  \param value   The value to put in to the register.
// */
inline static void CCPWrite( register8_t* address, uint8_t value )
{
   CriticalSection doNotInterrupt;
    #ifdef RAMPZ
   RAMPZ = 0;
    #endif
   CCP = CCP_IOREG_gc;
   *address = value;

}

/* as long as compiler.h from asf is included
   inline void barrier()
   {
    __asm__ volatile ("" ::: "memory");
   }
 */

inline void fatalErrorLoop()
{
   while ( 1 )
   {
   }
}

#endif
