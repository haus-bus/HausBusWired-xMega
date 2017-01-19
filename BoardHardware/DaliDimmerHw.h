/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: DaliDimmerHw
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Electronics/HwUnitBoards/DaliDimmerHw.h
*********************************************************************/

#ifndef Electronics_HwUnitBoards_DaliDimmerHw_H
#define Electronics_HwUnitBoards_DaliDimmerHw_H

//## auto_generated
#include "HwUnitBoards.h"
//## attribute daliHw
#include <Dali.h>
//## class DaliDimmerHw
#include <IDimmerHw.h>
//## package Electronics::HwUnitBoards

//## class DaliDimmerHw
class DaliDimmerHw : public IDimmerHw {
public :

    //## type Status
    struct Status {
        uint8_t error : 1;		//## attribute error
        uint8_t lampFailture : 1;		//## attribute lampFailture
        uint8_t lampEffort : 1;		//## attribute lampEffort
        uint8_t boundaryError : 1;		//## attribute boundaryError
        uint8_t steppingFinished : 1;		//## attribute steppingFinished
        uint8_t resetState : 1;		//## attribute resetState
        uint8_t addressMissing : 1;		//## attribute addressMissing
        uint8_t powerFailture : 1;		//## attribute powerFailture
    };
    
    ////    Constructors and destructors    ////
    
    //## operation DaliDimmerHw(uint8_t,Dali)
    DaliDimmerHw(uint8_t _address, Dali& _daliHw);
    
    ////    Operations    ////
    
    //## operation hasError()
    virtual uint8_t hasError();
    
    //## operation isOn()
    virtual uint16_t isOn();
    
    //## operation on(uint8_t)
    virtual void on(uint8_t value);
    
    //## operation setMode(uint8_t)
    virtual uint8_t setMode(uint8_t mode);
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline uint8_t getAddress() const {
        //#[ auto_generated
        return address;
        //#]
    }
    
    //## auto_generated
    inline void setAddress(uint8_t p_address) {
        //#[ auto_generated
        address = p_address;
        //#]
    }

protected :

    //## auto_generated
    inline static const uint8_t getDebugLevel() {
        //#[ auto_generated
        return debugLevel;
        //#]
    }

private :

    //## auto_generated
    inline Dali& getDaliHw() const {
        //#[ auto_generated
        return daliHw;
        //#]
    }
    
    //## auto_generated
    inline void setDaliHw(Dali& p_daliHw) {
        //#[ auto_generated
        daliHw = p_daliHw;
        //#]
    }
    
    ////    Attributes    ////

public :

    uint8_t address;		//## attribute address

protected :

    static const uint8_t debugLevel;		//## attribute debugLevel

private :

    Dali& daliHw;		//## attribute daliHw
};

#endif
/*********************************************************************
	File Path	: AR8/release/Electronics/HwUnitBoards/DaliDimmerHw.h
*********************************************************************/
