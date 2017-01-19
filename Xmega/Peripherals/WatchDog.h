/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: WatchDog
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/WatchDog.h
*********************************************************************/

#ifndef Peripherals_WatchDog_H
#define Peripherals_WatchDog_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## package Peripherals

//## class WatchDog
class WatchDog {
public :

    //#[ type Timeout
    enum Timeout {
      _8MS   = WDT_PER_8CLK_gc,
      _16MS  = WDT_PER_16CLK_gc,
      _32MS  = WDT_PER_32CLK_gc,
      _64MS  = WDT_PER_64CLK_gc,
      _125MS = WDT_PER_128CLK_gc,
      _250MS = WDT_PER_256CLK_gc,
      _500MS = WDT_PER_512CLK_gc,
      _1S    = WDT_PER_1KCLK_gc,
      _2S    = WDT_PER_2KCLK_gc,
      _4S    = WDT_PER_4KCLK_gc,
      _8S    = WDT_PER_8KCLK_gc,
    };
    //#]
    
    ////    Operations    ////
    
    // /*! \brief This function lock the entire clock system configuration.
    //  *
    //  *  This will lock the configuration until the next reset, or until the
    //  *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
    //  *  and switches to internal 2MHz RC oscillator.
    //  */
    //## operation disable()
    inline static void disable() {
        //#[ operation disable()
        CCPWrite( &WDT.CTRL, (WDT.CTRL & ~WDT_ENABLE_bm) | WDT_CEN_bm );
        /*
        uint8_t temp = (WDT.CTRL & ~WDT_ENABLE_bm) | WDT_CEN_bm;
        CCP = CCP_IOREG_gc;
        WDT.CTRL = temp;    
        */
        //#]
    }
    
    // /*! \brief This function lock the entire clock system configuration.
    //  *
    //  *  This will lock the configuration until the next reset, or until the
    //  *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
    //  *  and switches to internal 2MHz RC oscillator.
    //  */
    //## operation enable(Timeout)
    inline static void enable(Timeout timeout) {
        //#[ operation enable(Timeout)
        CCPWrite( &WDT.CTRL, WDT_ENABLE_bm | WDT_CEN_bm | timeout );
        /*    
        uint8_t temp = WDT_ENABLE_bm | WDT_CEN_bm | timeout;  
        CCP = CCP_IOREG_gc;
        WDT.CTRL = temp;
        */
        // Wait for WD to synchronize with new settings.
        while( isSyncBusy() );
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
        asm("wdr");
        //#]
    }

private :

    // /*! \brief This function lock the entire clock system configuration.
    //  *
    //  *  This will lock the configuration until the next reset, or until the
    //  *  External Oscillator Failure Detections (XOSCFD) feature detects a failure
    //  *  and switches to internal 2MHz RC oscillator.
    //  */
    //## operation isSyncBusy()
    inline static bool isSyncBusy() {
        //#[ operation isSyncBusy()
        return ( WDT.STATUS & WDT_SYNCBUSY_bm );
        //#]
    }
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/WatchDog.h
*********************************************************************/
