/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: KnxHw
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/HwProtocols/Knx/KnxHw.h
*********************************************************************/

#ifndef HwProtocols_Knx_KnxHw_H
#define HwProtocols_Knx_KnxHw_H

//## operation KnxHw(DigitalInput,DigitalOutput)
#include "DigitalInput.h"
//## operation KnxHw(DigitalInput,DigitalOutput)
#include "DigitalOutput.h"
//## operation write(uint8_t,uint16_t)
#include <Stream.h>
//## package HwProtocols::Knx

//## class KnxHw
class KnxHw {
    ////    Constructors and destructors    ////
    
public :

    //## operation KnxHw(DigitalInput,DigitalOutput)
    KnxHw(DigitalInput _digitalIn, DigitalOutput _digitalOut);
    
    ////    Operations    ////
    
    //## operation readByte()
    void readByte();
    
    //## operation write(uint8_t,uint16_t)
    Stream::Status write(uint8_t* pData, uint16_t length);

private :

    //## operation writeByte(uint8_t)
    Stream::Status writeByte(uint8_t data);
    
    ////    Additional operations    ////
    
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
	File Path	: Xmega192A3/debug/HwProtocols/Knx/KnxHw.h
*********************************************************************/
