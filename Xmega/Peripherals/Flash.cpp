/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: Flash
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/Flash.cpp
*********************************************************************/

//## auto_generated
#include "Peripherals/Flash.h"
//## package Peripherals

//## class Flash
Stream::Status Flash::read(Flash::address_t address, void * pData, uint16_t length) {
    //#[ operation read(address_t,void *,uint16_t)
    if( (address+length) > FLASHEND )
      return Stream::ABORTED;
    
    uint8_t* destination = static_cast<uint8_t*>(pData);
    
    while( length-- )
    {
      *destination++ = read( address ); 
    }     
    
    return Stream::SUCCESS;
    //#]
}

Stream::Status Flash::write(Flash::address_t address, void * pData, uint16_t length) {
    //#[ operation write(address_t,void *,uint16_t)
    if( (address & (getPageSize()-1)) || ( (address+length) > getMaxApplicationSize() ) )
    { 
      return Stream::ABORTED;
    }
    
    loadFlashPageBuffer( reinterpret_cast<uint8_t*>(pData) );
    eraseWriteApplicationPage( address );
    enableApplicationSection();
    
    return Stream::SUCCESS;
    //#]
}

/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/Flash.cpp
*********************************************************************/
