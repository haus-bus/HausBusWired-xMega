/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: Dali
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/HwProtocols/Dali.h
*********************************************************************/

#ifndef HwProtocols_Dali_H
#define HwProtocols_Dali_H

//## auto_generated
#include "HwProtocols.h"
//## operation Dali(DigitalInput,DigitalOutput)
#include "DigitalInput.h"
//## operation Dali(DigitalInput,DigitalOutput)
#include "DigitalOutput.h"
//## operation read(uint8_t)
#include <Stream.h>
//## package HwProtocols

//## class Dali
class Dali {
public :

    //#[ type MACROS
    // DALI uses 1200 baud, this waits for half of one bit time
    #define DALI_HALF_BIT_USEC 416
    #define DALI_HALF_BIT_WAIT _delay_us(DALI_HALF_BIT_USEC)  
    //#]
    
    //#[ type Commands
    enum Commands { 
        OFF                           = 0,
        TO_LIGHT,
        TO_DARK,
        STEP_TO_LIGHT, 
        STEP_TO_DARK,
        CALL_MAX_VALUE,
        CALL_MIN_VALUE,
        TO_DARK_AND_OFF,
        TO_LIGHT_AND_ON,    
        GOTO_SCENE_X                  = 16,    
        RESET                         = 32,
        STORE_VALUE_IN_DTR,    
        STORE_DTR_AS_MAX_VALUE        = 42,
        STORE_DTR_AS_MIN_VALUE,
        STORE_DTR_AS_SYSTEM_ERROR,
        STORE_DTR_AS_POWER_SUPPLY_ON,
        STORE_DTR_AS_STEP_TIME,
        STORE_DTR_AS_STEP_SPEED,
        STORE_DTR_AS_SCENE_X          = 64,    
        REMOVE_FROM_SCENE_X           = 80,    
        ADD_TO_GROUP_X                = 96,     
        REMOVE_FROM_GROUP_X           = 112,    
        STORE_DTR_AS_SCHORT_ADDR      = 128,
        GET_STATUS                    = 144,
        IS_DEVICE_OK,
        HAS_LAMP_ERROR,
        HAS_POWER,
        HAS_BOUNDARY_ERROR, 
        HAS_RESET_STATE,
        IS_SHORT_ADDR_MISSING, 
        GET_VERSION,
        GET_DTR_VALUE,
        GET_DEVICE_TYPE,
        GET_PHYSICAL_MIN_VALUE,
        HAS_SUPLLY_ERROR,
        GET_CURR_VALUE                  = 160,
        GET_MAX_VALUE,
        GET_MIN_VALUE,
        GET_POWER_ON_VALUE,
        GET_SYSTEM_ERROR_VALUE,
        GET_STEP_TIME_AND_SPEED,
        GET_SCENE_X_VALUE               = 176,
        GET_GROUP_1_TO_8_BELONGING      = 192, 
        GET_GROUP_9_TO_16_BELONGING,
        GET_DIRECT_ADDR_BYTE2,    
        GET_DIRECT_ADDR_BYTE1,
        GET_DIRECT_ADDR_BYTE0
    };
    //#]
    
    ////    Constructors and destructors    ////
    
    //## operation Dali(DigitalInput,DigitalOutput)
    Dali(DigitalInput _digitalIn, DigitalOutput _digitalOut);
    
    ////    Operations    ////
    
    //## operation isConnected()
    bool isConnected();
    
    //## operation read(uint8_t)
    Stream::Status read(uint8_t* pData);
    
    //## operation write(uint8_t)
    Stream::Status write(uint8_t* pData);
    
    ////    Additional operations    ////

private :

    //## auto_generated
    inline DigitalInput* getDigitalIn() const {
        //#[ auto_generated
        return (DigitalInput*) &digitalIn;
        //#]
    }
    
    //## auto_generated
    inline DigitalOutput* getDigitalOut() const {
        //#[ auto_generated
        return (DigitalOutput*) &digitalOut;
        //#]
    }
    
    ////    Attributes    ////
    
    DigitalInput digitalIn;		//## attribute digitalIn
    
    DigitalOutput digitalOut;		//## attribute digitalOut
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/HwProtocols/Dali.h
*********************************************************************/
