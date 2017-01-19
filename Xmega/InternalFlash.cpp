/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: InternalFlash
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/InternalFlash.cpp
*********************************************************************/

//## auto_generated
#include "InternalFlash.h"
//## package Basics

//## class InternalFlash
Stream::Status InternalFlash::read(uint16_t offset, void * pData, uint16_t length) {
    //#[ operation read(uint16_t,void *,uint16_t)
    return Flash::read( offset, pData, length );
    //#]
}

Stream::Status InternalFlash::write(uint16_t offset, void * pData, uint16_t length) {
    //#[ operation write(uint16_t,void *,uint16_t)
    return Flash::write( offset, pData, length );
    //#]
}

/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/InternalFlash.cpp
*********************************************************************/
