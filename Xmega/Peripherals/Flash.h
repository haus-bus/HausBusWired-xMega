/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: Flash
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/Flash.h
*********************************************************************/

#ifndef Peripherals_Flash_H
#define Peripherals_Flash_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## class Flash
#include "Peripherals/NvmController.h"
//## dependency pgmspace
#include <avr/pgmspace.h>
//## operation read(address_t,void *,uint16_t)
#include <Stream.h>
//## package Peripherals

//## class Flash

//#[ ignore
#ifndef _BOOTLOADER_SIZE_
#define _BOOTLOADER_SIZE_ 0
#endif
//#]


class Flash : public NvmController {
public :

    //#[ type address_t
    #if(FLASHEND > USHRT_MAX)
    typedef uint32_t address_t;
    #else
    typedef uint16_t address_t;
    #endif
    //#]
    
    ////    Operations    ////
    
    //## operation enableApplicationSection()
    inline static void enableApplicationSection();
    
    //## operation eraseApplication()
    inline static void eraseApplication();
    
    //## operation getMaxApplicationSize()
    inline static uint32_t getMaxApplicationSize();
    
    //## operation read(address_t,void *,uint16_t)
    static Stream::Status read(address_t address, void * pData, uint16_t length);
    
    //## operation read(address_t)
    inline static uint8_t read(address_t& address);
    
    //## operation write(address_t,void *,uint16_t)
    static Stream::Status write(address_t address, void * pData, uint16_t length);
    
    //## operation writeBootSection(address_t,void *,uint16_t)
    inline static void writeBootSection(address_t address, void * pData, uint16_t length);

private :

    //## operation eraseApplicationPage(address_t)
    inline static void eraseApplicationPage(address_t address);
    
    //## operation eraseWriteApplicationPage(uint32_t)
    inline static void eraseWriteApplicationPage(uint32_t& address) {
        //#[ operation eraseWriteApplicationPage(uint32_t)
        waitWhileBusy();
        NVM_commonSPM( address, NVM_CMD_ERASE_WRITE_APP_PAGE_gc );  
        //#]
    }
    
    // /**
    //  * \brief Write a page within the application section
    //  *
    //  * Write a page within the application section with the data stored in the
    //  * page buffer. The page needs to be erased before the write to avoid
    //  * corruption of the data written.
    //  *
    //  * \param page_addr Byte address to the page to delete
    //  */
    //## operation writeApplicationPage(address_t)
    inline static void writeApplicationPage(address_t& address);
};

inline void Flash::enableApplicationSection() {
    //#[ operation enableApplicationSection()
    NVM_CMD = NVM_CMD_NO_OPERATION_gc;
    //#]
}

inline void Flash::eraseApplication() {
    //#[ operation eraseApplication()
    waitWhileBusy();
    NVM_commonSPM( 0, NVM_CMD_ERASE_APP_gc );
    //#]
}

inline uint32_t Flash::getMaxApplicationSize() {
    //#[ operation getMaxApplicationSize()
    return APP_SECTION_SIZE;
    //#]
}

inline uint8_t Flash::read(Flash::address_t& address) {
    //#[ operation read(address_t)
    #if(FLASHEND > USHRT_MAX)
    return pgm_read_byte_far( address++ ); 
    #else
    return pgm_read_byte( address++ ); 
    #endif
    
    //#]
}

inline void Flash::writeBootSection(Flash::address_t address, void * pData, uint16_t length) {
    //#[ operation writeBootSection(address_t,void *,uint16_t)
    loadFlashPageBuffer( reinterpret_cast<uint8_t*>(pData) );
    eraseWriteBootPage( address );
    //#]
}

inline void Flash::eraseApplicationPage(Flash::address_t address) {
    //#[ operation eraseApplicationPage(address_t)
    waitWhileBusy();
    NVM_commonSPM( address, NVM_CMD_ERASE_APP_PAGE_gc );     
    //#]
}

inline void Flash::writeApplicationPage(Flash::address_t& address) {
    //#[ operation writeApplicationPage(address_t)
    waitWhileBusy();
    NVM_commonSPM( address, NVM_CMD_WRITE_APP_PAGE_gc );  
    //#]
}

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/Flash.h
*********************************************************************/
