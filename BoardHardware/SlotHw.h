/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: SlotHw
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Electronics/HwUnitBoards/SlotHw.h
*********************************************************************/

#ifndef Electronics_HwUnitBoards_SlotHw_H
#define Electronics_HwUnitBoards_SlotHw_H

//## auto_generated
#include "HwUnitBoards.h"
//## classInstance digitalOutput0
#include <DigitalOutput.h>
//## class SlotHw
#include <IDimmerHw.h>
//## attribute timerCounter0
#include <Peripherals/TimerCounter0.h>
//## dependency Logger
class Logger;

//## package Electronics::HwUnitBoards

//## class SlotHw
class SlotHw : public IDimmerHw {
public :

    //## type SlotType
    enum SlotType {
        UNUSED_SLOT,
        DIMMER,
        POWER_SOCKET,
        ROLLER_SHUTTER,
        DOUBLE_SWITCH = 0x07,
        DIMMER_30 = 0x0A,
        DIMMER_31 = 0x0B,
        MAX_SLOT_TYPES
    };
    
    //## type Versions
    enum Versions {
        V30 = 30,
        V31 = 31
    };
    
    //## type Modes
    enum Modes {
        DIMM_CR = 0,
        DIMM_L = 1,
        SWITCH = 2
    };
    
    ////    Constructors and destructors    ////
    
    //## operation SlotHw()
    SlotHw();
    
    ////    Operations    ////
    
    //## operation configure(uint8_t)
    void configure(uint8_t targetType);
    
    //## operation configureOutput(DigitalOutput,uint8_t)
    static void configureOutput(DigitalOutput& output, uint8_t isOutput);
    
    //## operation getPwmDuty()
    uint16_t getPwmDuty();
    
    //## operation hasError()
    uint8_t hasError();
    
    //## operation isDimmerHw()
    inline uint8_t isDimmerHw();
    
    //## operation isDoubleSwitchHw()
    inline bool isDoubleSwitchHw();
    
    //## operation isOn()
    uint16_t isOn();
    
    //## operation isPowerSocketHw()
    inline bool isPowerSocketHw();
    
    //## operation isRollerShutterHw()
    inline bool isRollerShutterHw();
    
    //## operation on(uint16_t)
    void on(uint16_t value);
    
    //## operation setMode(uint8_t)
    virtual uint8_t setMode(uint8_t mode);
    
    //## operation setPwmDuty(uint16_t)
    void setPwmDuty(uint16_t duty);
    
    //## operation stop()
    void stop();
    
    //## operation suspend(uint8_t)
    virtual uint8_t suspend(uint8_t mode);
    
    ////    Additional operations    ////
    
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
    inline uint8_t getType() const {
        //#[ auto_generated
        return type;
        //#]
    }
    
    //## auto_generated
    inline void setType(uint8_t p_type) {
        //#[ auto_generated
        type = p_type;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getPwmEnabled() const {
        //#[ auto_generated
        return pwmEnabled;
        //#]
    }
    
    //## auto_generated
    inline void setPwmEnabled(uint8_t p_pwmEnabled) {
        //#[ auto_generated
        pwmEnabled = p_pwmEnabled;
        //#]
    }
    
    //## auto_generated
    DigitalOutput* getDigitalOutput0() const;
    
    //## auto_generated
    DigitalOutput* getDigitalOutput1() const;

protected :

    //## auto_generated
    inline static const uint8_t getDebugLevel() {
        //#[ auto_generated
        return debugLevel;
        //#]
    }
    
    ////    Attributes    ////
    
    static const uint8_t debugLevel;		//## attribute debugLevel

public :

    TimerCounter0* timerCounter0;		//## attribute timerCounter0
    
    uint8_t type : 4;		//## attribute type
    
    uint8_t pwmEnabled : 1;		//## attribute pwmEnabled
    
    ////    Relations and components    ////

protected :

    DigitalOutput digitalOutput0;		//## classInstance digitalOutput0
    
    DigitalOutput digitalOutput1;		//## classInstance digitalOutput1
};

inline uint8_t SlotHw::isDimmerHw() {
    //#[ operation isDimmerHw()
    if (type == DIMMER) return true;
    if (type == DIMMER_30) return V30;
    if (type == DIMMER_31) return V31;
    return false;
    //#]
}

inline bool SlotHw::isDoubleSwitchHw() {
    //#[ operation isDoubleSwitchHw()
    return (type == DOUBLE_SWITCH);
    //#]
}

inline bool SlotHw::isPowerSocketHw() {
    //#[ operation isPowerSocketHw()
    return (type == POWER_SOCKET);
    //#]
}

inline bool SlotHw::isRollerShutterHw() {
    //#[ operation isRollerShutterHw()
    return (type == ROLLER_SHUTTER);
    //#]
}

#endif
/*********************************************************************
	File Path	: AR8/release/Electronics/HwUnitBoards/SlotHw.h
*********************************************************************/
