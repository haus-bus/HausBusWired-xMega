/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: TimerCounter1
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/TimerCounter1.h
*********************************************************************/

#ifndef Peripherals_TimerCounter1_H
#define Peripherals_TimerCounter1_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## class TimerCounter1
#include "Peripherals/TimerCounterBase.h"
//## package Peripherals

//## class TimerCounter1
class TimerCounter1 : public TimerCounterBase {
public :

    //#[ type TC1_t
    typedef struct TC1_struct
    {
      register8_t reserved_0x2C;
      register8_t reserved_0x2D;
      register8_t reserved_0x2E;
      register8_t reserved_0x2F;
      register8_t reserved_0x30;
      register8_t reserved_0x31;
      register8_t reserved_0x32;
      register8_t reserved_0x33;
      register8_t reserved_0x34;
      register8_t reserved_0x35;
      _WORDREGISTER(PERBUF);  /* Period Buffer */
      _WORDREGISTER(CCABUF);  /* Compare Or Capture A Buffer */
      _WORDREGISTER(CCBBUF);  /* Compare Or Capture B Buffer */
    } TC1_t;    
    //#]
    
    ////    Constructors and destructors    ////

protected :

    //## operation TimerCounter1()
    inline TimerCounter1() {
        //#[ operation TimerCounter1()
        //#]
    }
    
    ////    Operations    ////

public :

    //## operation disableChannels(uint8_t)
    inline void disableChannels(uint8_t disableMask) {
        //#[ operation disableChannels(uint8_t)
        // Make sure only CCxEN bits are set in disableMask. 
        disableMask &= ( TC1_CCAEN_bm | TC1_CCBEN_bm  );
        
        // Disable channels. 
        baseReg.CTRLB &= ~disableMask;
        //#]
    }
    
    //## operation enableChannels(uint8_t)
    inline void enableChannels(uint8_t enableMask) {
        //#[ operation enableChannels(uint8_t)
        // Make sure only CCxEN bits are set in enableMask.
        enableMask &= ( TC1_CCAEN_bm | TC1_CCBEN_bm );
        
        // Enable channels.
        baseReg.CTRLB |= enableMask;
        //#]
    }
    
    //## operation instance(uint8_t)
    static TimerCounter1& instance(uint8_t portNumber);
    
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
    
    //## operation setPeriodBuffered(uint16_t)
    inline void setPeriodBuffered(uint16_t value) {
        //#[ operation setPeriodBuffered(uint16_t)
        reg.PERBUF = value;
        //#]
    }
    
    ////    Attributes    ////

protected :

    TC1_t reg;		//## attribute reg
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/TimerCounter1.h
*********************************************************************/
