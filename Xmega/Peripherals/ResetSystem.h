/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: ResetSystem
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/ResetSystem.h
*********************************************************************/

#ifndef Peripherals_ResetSystem_H
#define Peripherals_ResetSystem_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## package Peripherals

//## class ResetSystem
class ResetSystem {
    ////    Operations    ////
    
public :

    // /*! \brief This function lock the entire clock system configuration.
    //  *
    //  *  This will lock the configuration until the next reset, or until the
    //  *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
    //  *  and switches to internal 2MHz RC oscillator.
    //  */
    //## operation clearSources(uint8_t)
    inline static void clearSources(uint8_t sources = RST_SDRF_bm|RST_SRF_bm|RST_PDIRF_bm|RST_WDRF_bm|RST_BORF_bm|RST_EXTRF_bm|RST_PORF_bm) {
        //#[ operation clearSources(uint8_t)
        RST.STATUS = sources;
        //#]
    }
    
    // /*! \brief This function lock the entire clock system configuration.
    //  *
    //  *  This will lock the configuration until the next reset, or until the
    //  *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
    //  *  and switches to internal 2MHz RC oscillator.
    //  */
    //## operation getSources()
    inline static uint8_t getSources() {
        //#[ operation getSources()
        return RST.STATUS;
        //#]
    }
    
    //## operation isBrownOutReset()
    inline static uint8_t isBrownOutReset() {
        //#[ operation isBrownOutReset()
        return getSources() & RST_BORF_bm;
        //#]
    }
    
    //## operation isDebuggerReset()
    inline static uint8_t isDebuggerReset() {
        //#[ operation isDebuggerReset()
        return getSources() & RST_PDIRF_bm;
        //#]
    }
    
    //## operation isExternalReset()
    inline static uint8_t isExternalReset() {
        //#[ operation isExternalReset()
        return getSources() & RST_EXTRF_bm;
        //#]
    }
    
    //## operation isPowerOnReset()
    inline static uint8_t isPowerOnReset() {
        //#[ operation isPowerOnReset()
        return getSources() & RST_PORF_bm;
        //#]
    }
    
    //## operation isSoftwareReset()
    inline static uint8_t isSoftwareReset() {
        //#[ operation isSoftwareReset()
        return getSources() & RST_SRF_bm;
        //#]
    }
    
    //## operation isSpikeDetectorReset()
    inline static uint8_t isSpikeDetectorReset() {
        //#[ operation isSpikeDetectorReset()
        return getSources() & RST_SDRF_bm;
        //#]
    }
    
    //## operation isWatchDogReset()
    inline static uint8_t isWatchDogReset() {
        //#[ operation isWatchDogReset()
        return getSources() & RST_WDRF_bm;
        //#]
    }
    
    // /*! \brief This function lock the entire clock system configuration.
    //  *
    //  *  This will lock the configuration until the next reset, or until the
    //  *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
    //  *  and switches to internal 2MHz RC oscillator.
    //  */
    //## operation reset()
    inline static void reset() {
        //#[ operation reset()
        CCPWrite( &RST.CTRL, RST_SWRST_bm );
        /*
        CCP = CCP_IOREG_gc;
        RST.CTRL = RST_SWRST_bm;
        */
        //#]
    }
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/ResetSystem.h
*********************************************************************/
