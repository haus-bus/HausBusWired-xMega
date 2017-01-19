/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: InternalEeprom
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/InternalEeprom.cpp
*********************************************************************/

//## auto_generated
#include "InternalEeprom.h"
//## dependency Eeprom
#include "Peripherals/Eeprom.h"
//## package Basics

//## class InternalEeprom
Stream::Status InternalEeprom::read(uint16_t offset, void * pData, uint16_t length) {
    //#[ operation read(uint16_t,void *,uint16_t)
    return Eeprom::read( offset, pData, length );
    //#]
}

Stream::Status InternalEeprom::write(uint16_t offset, void * pData, uint16_t length) {
    //#[ operation write(uint16_t,void *,uint16_t)
    return Eeprom::write( offset, pData, length );
    //#]
}

/*********************************************************************
	File Path	: Xmega192A3/debug/Basics/InternalEeprom.cpp
*********************************************************************/
