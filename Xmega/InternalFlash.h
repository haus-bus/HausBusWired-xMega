/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: InternalFlash
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/InternalFlash.h
*********************************************************************/

#ifndef Basics_InternalFlash_H
#define Basics_InternalFlash_H

//## auto_generated
#include "Basics.h"
//## dependency Flash
#include "Peripherals/Flash.h"
//## class InternalFlash
#include <StorageInterface.h>
//## operation read(uint16_t,void *,uint16_t)
#include <Stream.h>
//## package Basics

//## class InternalFlash
class InternalFlash : public StorageInterface {
    ////    Operations    ////
    
public :

    //## operation read(uint16_t,void *,uint16_t)
    virtual Stream::Status read(uint16_t offset, void * pData, uint16_t length);
    
    //## operation write(uint16_t,void *,uint16_t)
    virtual Stream::Status write(uint16_t offset, void * pData, uint16_t length);
};

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/InternalFlash.h
*********************************************************************/
