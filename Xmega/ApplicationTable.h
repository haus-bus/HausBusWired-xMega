/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: ApplicationTable
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/ApplicationTable.h
*********************************************************************/

#ifndef Basics_ApplicationTable_H
#define Basics_ApplicationTable_H

//## auto_generated
#include "Basics.h"
//## dependency Flash
#include "Peripherals/Flash.h"
//## operation read(uint16_t,void *,uint16_t)
#include <Stream.h>
//## package Basics

//## class ApplicationTable
class ApplicationTable {
public :

    //#[ type APPTABLE_SECTION_ADDR
    #ifndef APPTABLE_SECTION_ADDR
    #define APPTABLE_SECTION_ADDR APPTABLE_SECTION_START
    #endif
    //#]
    
    ////    Operations    ////
    
    //## operation read(uint16_t)
    inline static uint8_t read(uint16_t offset);
    
    //## operation read(uint16_t,void *,uint16_t)
    inline static Stream::Status read(uint16_t offset, void * pData, uint16_t length);
    
    //## operation write(uint16_t,void *,uint16_t)
    inline static Stream::Status write(uint16_t offset, void * pData, uint16_t length);
};

inline uint8_t ApplicationTable::read(uint16_t offset) {
    //#[ operation read(uint16_t)
    Flash::address_t address = APPTABLE_SECTION_ADDR + offset;
    return Flash::read( address );
    
    //#]
}

inline Stream::Status ApplicationTable::read(uint16_t offset, void * pData, uint16_t length) {
    //#[ operation read(uint16_t,void *,uint16_t)
    return Flash::read( APPTABLE_SECTION_ADDR + offset, pData, length );
    //#]
}

inline Stream::Status ApplicationTable::write(uint16_t offset, void * pData, uint16_t length) {
    //#[ operation write(uint16_t,void *,uint16_t)
    uint8_t buffer[Flash::getPageSize()];
    uint16_t index = offset % sizeof(buffer);        
    if( (index+length) > sizeof(buffer) )
    {
      return Stream::INVALID_BUFFER;
    }
    
    Flash::address_t address = APPTABLE_SECTION_ADDR + (offset&~index);
    Flash::read(address, buffer , sizeof(buffer) ); 
    memcpy( &buffer[index], pData, length );
    return Flash::write( address, buffer, sizeof(buffer) );
    //#]
}

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/ApplicationTable.h
*********************************************************************/
