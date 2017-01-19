/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: Peripherals
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/Peripherals.h
*********************************************************************/

#ifndef Peripherals_Peripherals_H
#define Peripherals_Peripherals_H

//## operation CCPWrite(register8_t,uint8_t)
#include "Basics.h"

//## dependency Logger
//#[ ignore
#ifdef _DEBUG_                                                         

//#]
#include "Traces/Logger.h"
//#[ ignore

#else  
#include "SwFramework.h"    
#endif
//#]
//## auto_generated
class Adc;

//## auto_generated
class Clock;

//## auto_generated
class DigitalFrequencyLockedLoops;

//## auto_generated
class DmaChannel;

//## auto_generated
class DmaController;

//## auto_generated
class Eeprom;

//## auto_generated
class EventSystem;

//## auto_generated
class Flash;

//## auto_generated
class InterruptController;

//## auto_generated
class IoPort;

//## auto_generated
class NvmController;

//## auto_generated
class Oscillator;

//## auto_generated
class RealTimeCounter;

//## auto_generated
class ResetSystem;

//## auto_generated
class Spi;

//## auto_generated
class TimerCounter0;

//## auto_generated
class TimerCounter1;

//## auto_generated
class TimerCounterBase;

//## auto_generated
class Twi;

//## auto_generated
class Usart;

//## auto_generated
class WatchDog;

//## package Peripherals


//#[ type TRACE_PORT
#define TRACE_PORT 0x80

#ifdef _TRACE_PORT_ 
  #define TRACE_PORT_INIT( pins )     _TRACE_PORT_.DIRSET = pins;    _TRACE_PORT_.OUTCLR = pins;
  #define TRACE_PORT_PUT( pins )      if( debugLevel & TRACE_PORT )  _TRACE_PORT_.OUT = pins;
  #define TRACE_PORT_SET( pins )      if( debugLevel & TRACE_PORT )  _TRACE_PORT_.OUTSET = pins;
  #define TRACE_PORT_CLEAR( pins )    if( debugLevel & TRACE_PORT )  _TRACE_PORT_.OUTCLR = pins;
  #define TRACE_PORT_TOGGLE( pins )   if( debugLevel & TRACE_PORT )  _TRACE_PORT_.OUTTGL = pins;
#else  
  #define TRACE_PORT_INIT( pins )  
  #define TRACE_PORT_PUT( pins )
  #define TRACE_PORT_SET( pins )     
  #define TRACE_PORT_CLEAR( pins )  
  #define TRACE_PORT_TOGGLE( pins )
#endif  
//#]

// /*! \brief CCP write helper function written in assembly.
//  *
//  *  This function is written in assembly because of the timecritial
//  *  operation of writing to the registers.
//  *
//  *  \param address A pointer to the address to write to.
//  *  \param value   The value to put in to the register.
//  */
//## operation CCPWrite(register8_t,uint8_t)
inline static void CCPWrite(register8_t* address, uint8_t value) {
    //#[ operation CCPWrite(register8_t,uint8_t)
    CriticalSection doNotInterrupt;  
    #ifdef RAMPZ
    	RAMPZ = 0;
    #endif
    CCP = CCP_IOREG_gc;
    *address = value;
    
    //#]
}

//## operation barrier()
inline void barrier() {
    //#[ operation barrier()
    __asm__ volatile ("" ::: "memory");
    //#]
}

//## operation fatalErrorLoop()
inline void fatalErrorLoop() {
    //#[ operation fatalErrorLoop()
    //TODO
    while(1);
    //#]
}

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/Peripherals.h
*********************************************************************/
