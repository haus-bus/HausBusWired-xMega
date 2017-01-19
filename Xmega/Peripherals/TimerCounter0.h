/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: TimerCounter0
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/TimerCounter0.h
*********************************************************************/

#ifndef Peripherals_TimerCounter0_H
#define Peripherals_TimerCounter0_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## class TimerCounter0
#include "Peripherals/TimerCounterBase.h"
//## package Peripherals

//## class TimerCounter0
class TimerCounter0 : public TimerCounterBase {
public :

    //#[ type TC0_t
    typedef struct TC0_struct
    {
      _WORDREGISTER(CCC);  /* Compare or Capture C */
      _WORDREGISTER(CCD);  /* Compare or Capture D */
      register8_t reserved_0x30;
      register8_t reserved_0x31;
      register8_t reserved_0x32;
      register8_t reserved_0x33;
      register8_t reserved_0x34;
      register8_t reserved_0x35;
      _WORDREGISTER(PERBUF);  /* Period Buffer */
      _WORDREGISTER(CCABUF);  /* Compare Or Capture A Buffer */
      _WORDREGISTER(CCBBUF);  /* Compare Or Capture B Buffer */
      _WORDREGISTER(CCCBUF);  /* Compare Or Capture C Buffer */
      _WORDREGISTER(CCDBUF);  /* Compare Or Capture D Buffer */
    } TC0_t;    
    //#]
    
    ////    Operations    ////
    
    //## operation clearCCCFlag()
    inline void clearCCCFlag() {
        //#[ operation clearCCCFlag()
        baseReg.INTFLAGS = TC0_CCCIF_bm;
        //#]
    }
    
    //## operation clearCCDFlag()
    inline void clearCCDFlag() {
        //#[ operation clearCCDFlag()
        baseReg.INTFLAGS = TC0_CCDIF_bm;
        //#]
    }
    
    //## operation disableChannels(uint8_t)
    inline void disableChannels(uint8_t disableMask) {
        //#[ operation disableChannels(uint8_t)
        // Make sure only CCxEN bits are set in disableMask. 
        disableMask &= ( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm );
        
        // Disable channels. 
        baseReg.CTRLB &= ~disableMask;
        //#]
    }
    
    //## operation enableChannels(uint8_t)
    inline void enableChannels(uint8_t enableMask) {
        //#[ operation enableChannels(uint8_t)
        // Make sure only CCxEN bits are set in enableMask.
        enableMask &= ( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm );
        
        // Enable channels.
        baseReg.CTRLB |= enableMask;
        //#]
    }
    
    //## operation getCCCFlag()
    inline uint8_t getCCCFlag() {
        //#[ operation getCCCFlag()
        return baseReg.INTFLAGS & TC0_CCCIF_bm;
        //#]
    }
    
    //## operation getCCDFlag()
    inline uint8_t getCCDFlag() {
        //#[ operation getCCDFlag()
        return baseReg.INTFLAGS & TC0_CCDIF_bm;
        //#]
    }
    
    //## operation getCaptureC()
    inline uint16_t getCaptureC() {
        //#[ operation getCaptureC()
        return reg.CCC;
        //#]
    }
    
    //## operation getCaptureD()
    inline uint16_t getCaptureD() {
        //#[ operation getCaptureD()
        return reg.CCD;
        //#]
    }
    
    //## operation instance(uint8_t)
    static TimerCounter0& instance(uint8_t portNumber);
    
    //## operation setCCCIntLevel(TC_CCCINTLVL_t)
    inline void setCCCIntLevel(TC_CCCINTLVL_t intLevel) {
        //#[ operation setCCCIntLevel(TC_CCCINTLVL_t)
        baseReg.INTCTRLB = ( baseReg.INTCTRLB & ~TC0_CCCINTLVL_gm ) | intLevel;
        //#]
    }
    
    //## operation setCCDIntLevel(TC_CCDINTLVL_t)
    inline void setCCDIntLevel(TC_CCDINTLVL_t intLevel) {
        //#[ operation setCCDIntLevel(TC_CCDINTLVL_t)
        baseReg.INTCTRLB = ( baseReg.INTCTRLB & ~TC0_CCDINTLVL_gm ) | intLevel;
        //#]
    }
    
    //## operation setCompareA(uint16_t)
    inline void setCompareA(uint16_t value) {
        //#[ operation setCompareA(uint16_t)
        reg.CCABUF = value;
        //#]
    }
    
    //## operation setCompareB(uint16_t)
    inline void setCompareB(uint16_t value) {
        //#[ operation setCompareB(uint16_t)
        reg.CCBBUF = value;
        //#]
    }
    
    //## operation setCompareC(uint16_t)
    inline void setCompareC(uint16_t value) {
        //#[ operation setCompareC(uint16_t)
        reg.CCCBUF = value;
        //#]
    }
    
    //## operation setCompareD(uint16_t)
    inline void setCompareD(uint16_t value) {
        //#[ operation setCompareD(uint16_t)
        reg.CCDBUF = value;
        //#]
    }
    
    //## operation setPeriodBuffered(uint16_t)
    inline void setPeriodBuffered(uint16_t value) {
        //#[ operation setPeriodBuffered(uint16_t)
        reg.PERBUF = value;
        //#]
    }
    
    ////    Attributes    ////

protected :

    TC0_t reg;		//## attribute reg
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/TimerCounter0.h
*********************************************************************/
