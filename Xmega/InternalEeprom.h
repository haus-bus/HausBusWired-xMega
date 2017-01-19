/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: InternalEeprom
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/InternalEeprom.h
*********************************************************************/

#ifndef Basics_InternalEeprom_H
#define Basics_InternalEeprom_H

//## auto_generated
#include "Basics.h"
//## class InternalEeprom
#include <StorageInterface.h>
//## operation read(uint16_t,void *,uint16_t)
#include <Stream.h>
//## dependency Eeprom
class Eeprom;

//## package Basics

//## class InternalEeprom
class InternalEeprom : public StorageInterface {
    ////    Constructors and destructors    ////
    
public :

    //## operation InternalEeprom()
    inline InternalEeprom() {
        //#[ operation InternalEeprom()
        //#]
    }
    
    ////    Operations    ////
    
    //## operation read(uint16_t,void *,uint16_t)
    virtual Stream::Status read(uint16_t offset, void * pData, uint16_t length);
    
    //## operation write(uint16_t,void *,uint16_t)
    virtual Stream::Status write(uint16_t offset, void * pData, uint16_t length);
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/InternalEeprom.h
*********************************************************************/
