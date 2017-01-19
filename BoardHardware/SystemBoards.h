/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: SystemBoards
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Electronics/SystemBoards/SystemBoards.h
*********************************************************************/

#ifndef Electronics_SystemBoards_SystemBoards_H
#define Electronics_SystemBoards_SystemBoards_H

//## operation lowLevelInit()
#include <DefaultTypes.h>
//## auto_generated
#include <Electronics.h>
//## dependency InterruptController
#include <Peripherals/InterruptController.h>
//## dependency IoPort
#include <Peripherals/IoPort.h>
//## dependency ResetSystem
#include <Peripherals/ResetSystem.h>
//## dependency Usart
#include <Peripherals/Usart.h>
//## dependency WatchDog
#include <Peripherals/WatchDog.h>
//## dependency Clock
class Clock;

//## auto_generated
class HomeAutomationHw;

//## dependency Oscillator
class Oscillator;

//## dependency RealTimeCounter
class RealTimeCounter;

//## package Electronics::SystemBoards


//## type TRACE_PORT_PINS
enum TRACE_PORT_PINS {
    TR_IDLE_PIN = Pin0,
    TR_INT_PIN = Pin1
};

//#[ type MOD_ID_SECTION
#define MOD_ID_SECTION __attribute__((section(".vectors")))
//#]

//#[ type MAX_SLOTS
static const uint8_t MAX_SLOTS = 8; 
//#]

#endif
/*********************************************************************
	File Path	: AR8/release/Electronics/SystemBoards/SystemBoards.h
*********************************************************************/
