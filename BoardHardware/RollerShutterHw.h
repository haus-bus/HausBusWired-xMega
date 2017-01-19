/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: RollerShutterHw
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Electronics/HwUnitBoards/RollerShutterHw.h
*********************************************************************/

#ifndef Electronics_HwUnitBoards_RollerShutterHw_H
#define Electronics_HwUnitBoards_RollerShutterHw_H

//## auto_generated
#include "HwUnitBoards.h"
//## class RollerShutterHw
#include "SlotHw.h"
//## dependency Logger
class Logger;

//## package Electronics::HwUnitBoards

//## class RollerShutterHw
class RollerShutterHw : public SlotHw {
public :

    //## class RollerShutterHw::ErrorCode
    class ErrorCode {
    public :
    
        //#[ type Errors
        enum Errors {
        	  NO_ERROR,
        	  INVALID_MOTOR_STATE,
        };
        //#]
    };
    
    ////    Operations    ////
    
    //## operation isDirectionToClose()
    bool isDirectionToClose();
    
    //## operation isDirectionToOpen()
    bool isDirectionToOpen();
    
    //## operation off()
    void off();
    
    //## operation on()
    void on();
    
    //## operation setDirectionToClose()
    void setDirectionToClose();
    
    //## operation setDirectionToOpen()
    void setDirectionToOpen();
    
    //## operation setInverted(bool)
    inline void setInverted(bool inverted);
};

inline void RollerShutterHw::setInverted(bool inverted) {
    //#[ operation setInverted(bool)
    digitalOutput0.setInverted( inverted );
    //#]
}

#endif
/*********************************************************************
	File Path	: AR8/release/Electronics/HwUnitBoards/RollerShutterHw.h
*********************************************************************/
