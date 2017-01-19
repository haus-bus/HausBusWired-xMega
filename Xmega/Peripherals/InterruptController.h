/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: InterruptController
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/InterruptController.h
*********************************************************************/

#ifndef Peripherals_InterruptController_H
#define Peripherals_InterruptController_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## package Peripherals

//## class InterruptController
class InterruptController {
    ////    Operations    ////
    
public :

    //## operation disableRoundRobinScheduling()
    inline static void disableRoundRobinScheduling() {
        //#[ operation disableRoundRobinScheduling()
        PMIC.CTRL &= ~PMIC_RREN_bm;
        //#]
    }
    
    //## operation enableRoundRobinScheduling()
    inline static void enableRoundRobinScheduling() {
        //#[ operation enableRoundRobinScheduling()
        PMIC.CTRL |= PMIC_RREN_bm;
        //#]
    }
    
    //## operation selectAppInterruptSection()
    inline static void selectAppInterruptSection() {
        //#[ operation selectAppInterruptSection()
        CCPWrite( &PMIC.CTRL, PMIC.CTRL & ~PMIC_IVSEL_bm );
        //#]
    }
    
    //## operation selectBootInterruptSection()
    inline static void selectBootInterruptSection() {
        //#[ operation selectBootInterruptSection()
        CCPWrite( &PMIC.CTRL, PMIC.CTRL | PMIC_IVSEL_bm );
        //#]
    }
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/InterruptController.h
*********************************************************************/
