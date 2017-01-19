/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: DigitalPortHw
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Electronics/HwUnitBoards/DigitalPortHw.h
*********************************************************************/

#ifndef Electronics_HwUnitBoards_DigitalPortHw_H
#define Electronics_HwUnitBoards_DigitalPortHw_H

//## auto_generated
#include "HwUnitBoards.h"
//## attribute port
#include <Peripherals/IoPort.h>
//## attribute timerCounter0
#include <Peripherals/TimerCounter0.h>
//## attribute timerCounter1
#include <Peripherals/TimerCounter1.h>
//## package Electronics::HwUnitBoards

//## class DigitalPortHw
class DigitalPortHw {
    ////    Constructors and destructors    ////
    
public :

    //## operation DigitalPortHw(uint8_t)
    DigitalPortHw(uint8_t portNumber);
    
    ////    Operations    ////
    
    //## operation configure(uint8_t,uint8_t,uint8_t)
    void configure(uint8_t outputPins, uint8_t inputPins, uint8_t pwmPins = 0);
    
    //## operation off(uint8_t)
    inline void off(uint8_t pin);
    
    //## operation on(uint8_t,uint8_t)
    void on(uint8_t pin, uint8_t value = 100);
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline IoPort& getPort() const {
        //#[ auto_generated
        return port;
        //#]
    }
    
    //## auto_generated
    inline void setPort(IoPort& p_port) {
        //#[ auto_generated
        port = p_port;
        //#]
    }
    
    //## auto_generated
    inline TimerCounter0* getTimerCounter0() const {
        //#[ auto_generated
        return timerCounter0;
        //#]
    }
    
    //## auto_generated
    inline void setTimerCounter0(TimerCounter0* p_timerCounter0) {
        //#[ auto_generated
        timerCounter0 = p_timerCounter0;
        //#]
    }
    
    //## auto_generated
    inline TimerCounter1* getTimerCounter1() const {
        //#[ auto_generated
        return timerCounter1;
        //#]
    }
    
    //## auto_generated
    inline void setTimerCounter1(TimerCounter1* p_timerCounter1) {
        //#[ auto_generated
        timerCounter1 = p_timerCounter1;
        //#]
    }
    
    ////    Attributes    ////
    
    IoPort& port;		//## attribute port
    
    TimerCounter0* timerCounter0;		//## attribute timerCounter0
    
    TimerCounter1* timerCounter1;		//## attribute timerCounter1
};

inline void DigitalPortHw::off(uint8_t pin) {
    //#[ operation off(uint8_t)
    on( pin, 0 );
    //#]
}

#endif
/*********************************************************************
	File Path	: AR8/release/Electronics/HwUnitBoards/DigitalPortHw.h
*********************************************************************/
