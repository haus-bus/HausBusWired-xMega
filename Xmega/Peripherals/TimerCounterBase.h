/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: TimerCounterBase
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/TimerCounterBase.h
*********************************************************************/

#ifndef Peripherals_TimerCounterBase_H
#define Peripherals_TimerCounterBase_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## package Peripherals

//## class TimerCounterBase
class TimerCounterBase {
public :

    //#[ type TCBase_t
    typedef struct TCBase_struct
    {
        register8_t CTRLA;  /* Control  Register A */
        register8_t CTRLB;  /* Control Register B */
        register8_t CTRLC;  /* Control register C */
        register8_t CTRLD;  /* Control Register D */
        register8_t CTRLE;  /* Control Register E */
        register8_t reserved_0x05;
        register8_t INTCTRLA;  /* Interrupt Control Register A */
        register8_t INTCTRLB;  /* Interrupt Control Register B */
        register8_t CTRLFCLR;  /* Control Register F Clear */
        register8_t CTRLFSET;  /* Control Register F Set */
        register8_t CTRLGCLR;  /* Control Register G Clear */
        register8_t CTRLGSET;  /* Control Register G Set */
        register8_t INTFLAGS;  /* Interrupt Flag Register */
        register8_t reserved_0x0D;
        register8_t reserved_0x0E;
        register8_t TEMP;  /* Temporary Register For 16-bit Access */
        register8_t reserved_0x10;
        register8_t reserved_0x11;
        register8_t reserved_0x12;
        register8_t reserved_0x13;
        register8_t reserved_0x14;
        register8_t reserved_0x15;
        register8_t reserved_0x16;
        register8_t reserved_0x17;
        register8_t reserved_0x18;
        register8_t reserved_0x19;
        register8_t reserved_0x1A;
        register8_t reserved_0x1B;
        register8_t reserved_0x1C;
        register8_t reserved_0x1D;
        register8_t reserved_0x1E;
        register8_t reserved_0x1F;
        _WORDREGISTER(CNT);  /* Count */  
        register8_t reserved_0x22;
        register8_t reserved_0x23;
        register8_t reserved_0x24;
        register8_t reserved_0x25;
        _WORDREGISTER(PER);  /* Period */ 
        _WORDREGISTER(CCA);  /* Compare or Capture A */
        _WORDREGISTER(CCB);  /* Compare or Capture B */
    } TCBase_t;    
    //#]
    
    ////    Constructors and destructors    ////

protected :

    //## operation TimerCounterBase()
    inline TimerCounterBase() {
        //#[ operation TimerCounterBase()
        //#]
    }
    
    ////    Operations    ////

public :

    //## operation clearCCAFlag()
    inline void clearCCAFlag() {
        //#[ operation clearCCAFlag()
        baseReg.INTFLAGS = TC0_CCAIF_bm;
        //#]
    }
    
    //## operation clearCCBFlag()
    inline void clearCCBFlag() {
        //#[ operation clearCCBFlag()
        baseReg.INTFLAGS = TC0_CCBIF_bm;
        //#]
    }
    
    //## operation clearErrorFlag()
    inline void clearErrorFlag() {
        //#[ operation clearErrorFlag()
        baseReg.INTFLAGS = TC0_ERRIF_bm;
        //#]
    }
    
    //## operation clearOverflowFlag()
    inline void clearOverflowFlag() {
        //#[ operation clearOverflowFlag()
        baseReg.INTFLAGS = TC0_OVFIF_bm;
        //#]
    }
    
    //## operation configClockSource(TC_CLKSEL_t)
    inline void configClockSource(TC_CLKSEL_t clockSelection) {
        //#[ operation configClockSource(TC_CLKSEL_t)
        baseReg.CTRLA = ( baseReg.CTRLA & ~TC0_CLKSEL_gm ) | clockSelection;
        //#]
    }
    
    //## operation configEventAction(TC_EVSEL_t,TC_EVACT_t)
    inline void configEventAction(TC_EVSEL_t eventSource, TC_EVACT_t eventAction) {
        //#[ operation configEventAction(TC_EVSEL_t,TC_EVACT_t)
        baseReg.CTRLD = ( baseReg.CTRLD & ~( TC0_EVSEL_gm | TC0_EVACT_gm ) ) 
                      |   eventSource 
                      |   eventAction;
        //#]
    }
    
    //## operation configInputCapture(TC_EVSEL_t)
    inline void configInputCapture(TC_EVSEL_t eventSource) {
        //#[ operation configInputCapture(TC_EVSEL_t)
        baseReg.CTRLD = ( baseReg.CTRLD & ~( TC0_EVSEL_gm | TC0_EVACT_gm ) ) 
                      |   eventSource 
                      |   TC_EVACT_CAPT_gc;
        //#]
    }
    
    //## operation configWGM(TC_WGMODE_t)
    inline void configWGM(TC_WGMODE_t wgm) {
        //#[ operation configWGM(TC_WGMODE_t)
        baseReg.CTRLB = ( baseReg.CTRLB & ~TC0_WGMODE_gm ) | wgm;
        //#]
    }
    
    //## operation disableEventDelay()
    inline void disableEventDelay() {
        //#[ operation disableEventDelay()
        baseReg.CTRLD &= ~TC0_EVDLY_bm;
        //#]
    }
    
    //## operation enableEventDelay()
    inline void enableEventDelay() {
        //#[ operation enableEventDelay()
        baseReg.CTRLD |= TC0_EVDLY_bm;
        //#]
    }
    
    //## operation forceUpdate()
    inline void forceUpdate() {
        //#[ operation forceUpdate()
        baseReg.CTRLFSET = TC_CMD_UPDATE_gc;
        //#]
    }
    
    //## operation getCCAFlag()
    inline uint8_t getCCAFlag() {
        //#[ operation getCCAFlag()
        return baseReg.INTFLAGS & TC0_CCAIF_bm;
        //#]
    }
    
    //## operation getCCBFlag()
    inline uint8_t getCCBFlag() {
        //#[ operation getCCBFlag()
        return baseReg.INTFLAGS & TC0_CCBIF_bm;
        //#]
    }
    
    //## operation getCaptureA()
    inline uint16_t getCaptureA() {
        //#[ operation getCaptureA()
        return baseReg.CCA;
        //#]
    }
    
    //## operation getCaptureB()
    inline uint16_t getCaptureB() {
        //#[ operation getCaptureB()
        return baseReg.CCB;
        //#]
    }
    
    //## operation getErrorFlag()
    inline uint8_t getErrorFlag() {
        //#[ operation getErrorFlag()
        return baseReg.INTFLAGS & TC0_ERRIF_bm;
        //#]
    }
    
    //## operation getOverflowFlag()
    inline uint8_t getOverflowFlag() {
        //#[ operation getOverflowFlag()
        return baseReg.INTFLAGS & TC0_OVFIF_bm;
        //#]
    }
    
    //## operation getPeriod()
    inline uint16_t getPeriod() {
        //#[ operation getPeriod()
        return baseReg.PER;
        //#]
    }
    
    //## operation isRunning()
    inline uint8_t isRunning() {
        //#[ operation isRunning()
        return baseReg.CTRLA;
        //#]
    }
    
    //## operation lockCompareUpdate()
    inline void lockCompareUpdate() {
        //#[ operation lockCompareUpdate()
        baseReg.CTRLFSET = TC0_LUPD_bm;
        //#]
    }
    
    //## operation reset()
    inline void reset() {
        //#[ operation reset()
        // TC must be turned off before a Reset command.
        baseReg.CTRLA = ( baseReg.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_OFF_gc;
        
        // Issue Reset command.
        baseReg.CTRLFSET = TC_CMD_RESET_gc;
        //#]
    }
    
    //## operation restart()
    inline void restart() {
        //#[ operation restart()
        baseReg.CTRLFSET = TC_CMD_RESTART_gc;
        //#]
    }
    
    //## operation setCCAIntLevel(TC_CCAINTLVL_t)
    inline void setCCAIntLevel(TC_CCAINTLVL_t intLevel) {
        //#[ operation setCCAIntLevel(TC_CCAINTLVL_t)
        baseReg.INTCTRLB = ( baseReg.INTCTRLB & ~TC0_CCAINTLVL_gm ) | intLevel;
        //#]
    }
    
    //## operation setCCBIntLevel(TC_CCBINTLVL_t)
    inline void setCCBIntLevel(TC_CCBINTLVL_t intLevel) {
        //#[ operation setCCBIntLevel(TC_CCBINTLVL_t)
        baseReg.INTCTRLB = ( baseReg.INTCTRLB & ~TC0_CCBINTLVL_gm ) | intLevel;
        //#]
    }
    
    //## operation setCount(uint16_t)
    inline void setCount(uint16_t value) {
        //#[ operation setCount(uint16_t)
        baseReg.CNT = value;
        //#]
    }
    
    //## operation setErrorIntLevel(TC_ERRINTLVL_t)
    inline void setErrorIntLevel(TC_ERRINTLVL_t intLevel) {
        //#[ operation setErrorIntLevel(TC_ERRINTLVL_t)
        baseReg.INTCTRLA = ( baseReg.INTCTRLA & ~TC0_ERRINTLVL_gm ) | intLevel;
        //#]
    }
    
    //## operation setOverflowIntLevel(TC_OVFINTLVL_t)
    inline void setOverflowIntLevel(TC_OVFINTLVL_t intLevel) {
        //#[ operation setOverflowIntLevel(TC_OVFINTLVL_t)
        baseReg.INTCTRLA = ( baseReg.INTCTRLA & ~TC0_OVFINTLVL_gm ) | intLevel;
        //#]
    }
    
    //## operation setPeriod(uint16_t)
    inline void setPeriod(uint16_t value) {
        //#[ operation setPeriod(uint16_t)
        baseReg.PER = value;
        //#]
    }
    
    //## operation unlockCompareUpdate()
    inline void unlockCompareUpdate() {
        //#[ operation unlockCompareUpdate()
        baseReg.CTRLFCLR = TC0_LUPD_bm;
        //#]
    }
    
    ////    Attributes    ////

protected :

    TCBase_t baseReg;		//## attribute baseReg
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/TimerCounterBase.h
*********************************************************************/
