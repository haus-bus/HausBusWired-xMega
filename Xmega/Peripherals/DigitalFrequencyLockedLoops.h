/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: DigitalFrequencyLockedLoops
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/DigitalFrequencyLockedLoops.h
*********************************************************************/

#ifndef Peripherals_DigitalFrequencyLockedLoops_H
#define Peripherals_DigitalFrequencyLockedLoops_H

//## auto_generated
#include "Peripherals.h"
//## package Peripherals

//## class DigitalFrequencyLockedLoops
class DigitalFrequencyLockedLoops {
public :

    ////    Constructors and destructors    ////

protected :

    //## operation DigitalFrequencyLockedLoops()
    inline DigitalFrequencyLockedLoops() {
        //#[ operation DigitalFrequencyLockedLoops()
        //#]
    }
    
    ////    Operations    ////

public :

    // /*! \brief This function disables the automatic calibration of the internal oscillator.
    //  *
    //  */
    //## operation disableAutoCalibration()
    inline void disableAutoCalibration() {
        //#[ operation disableAutoCalibration()
        reg.CTRL &= ~DFLL_ENABLE_bm;
        //#]
    }
    
    //## operation instance(bool)
    static DigitalFrequencyLockedLoops& instance(bool for32MHz);
    
    ////    Attributes    ////

protected :

    DFLL_t reg;		//## attribute reg
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/DigitalFrequencyLockedLoops.h
*********************************************************************/
