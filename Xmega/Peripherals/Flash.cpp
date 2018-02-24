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

volatile char i;

Stream::Status Flash::write(Flash::address_t address, void * pData, uint16_t length) {
    //#[ operation write(address_t,void *,uint16_t)
    if(  (address+length) > getMaxApplicationSize() )
    {
      return Stream::ABORTED;
    }
    uint8_t buffer[APP_SECTION_PAGE_SIZE];
    uint16_t offset = address & (sizeof(buffer)-1);
    address -= offset;

    // fill the buffer with the current data and update the new part
    Flash::read( address, buffer, sizeof(buffer) );
    memcpy(&buffer[offset], pData, length);

    if( address >= (Flash::address_t) 0x10000 )
    {
      address |= 0x10000000;
    }

    loadFlashPageBuffer( buffer );
    eraseWriteApplicationPage( address );
    enableApplicationSection();

    return Stream::SUCCESS;
    //#]
}

/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/Flash.cpp
*********************************************************************/
