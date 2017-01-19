/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: NvmController
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/NvmController.h
*********************************************************************/

#ifndef Peripherals_NvmController_H
#define Peripherals_NvmController_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## dependency NvmLowLevel
#include "Peripherals/NvmLowLevel.h"
//## operation readUserSignature(uint16_t,void *,uint16_t)
#include <Stream.h>
//## package Peripherals

//## class NvmController
class NvmController {
public :

    ////    Operations    ////
    
    //## operation eraseUserSignature()
    inline static void eraseUserSignature();
    
    //## operation executeCommand(NVM_CMD_t)
    static void executeCommand(NVM_CMD_t command);
    
    //## operation getApplicationCRC()
    inline static uint32_t getApplicationCRC() {
        //#[ operation getApplicationCRC()
        executeCommand( NVM_CMD_APP_CRC_gc ); 
        uint32_t crc = ((uint32_t)NVM.DATA2<<16) | ((uint16_t)NVM.DATA1<<8) | NVM.DATA0;
        NVM_CMD = NVM_CMD_NO_OPERATION_gc;
        return crc; 
        //#]
    }
    
    //## operation getBootCRC()
    inline static uint32_t getBootCRC() {
        //#[ operation getBootCRC()
        executeCommand( NVM_CMD_BOOT_CRC_gc );
        uint32_t crc = ((uint32_t)NVM.DATA2<<16) | ((uint16_t)NVM.DATA1<<8) | NVM.DATA0;
        NVM_CMD = NVM_CMD_NO_OPERATION_gc;
        return crc; 
        //#]
    }
    
    //## operation getMaxBooterSize()
    inline static uint32_t getMaxBooterSize();
    
    //## operation getPageSize()
    inline static uint16_t getPageSize();
    
    //## operation getRangeCRC(uint32_t,uint32_t)
    inline static uint32_t getRangeCRC(const uint32_t& start, const uint32_t& end) {
        //#[ operation getRangeCRC(uint32_t,uint32_t)
        // Set address.
        NVM.ADDR0 =  start & 0xFF;
        NVM.ADDR1 = (start >> 8) & 0xFF;
        NVM.ADDR2 = (start >> 16) & 0xFF;    
        
        NVM.DATA0 =  end & 0xFF;
        NVM.DATA1 = (end >> 8) & 0xFF;
        NVM.DATA2 = (end >> 16) & 0xFF;
        
        executeCommand( NVM_CMD_FLASH_RANGE_CRC_gc );
        uint32_t crc = ((uint32_t)NVM.DATA2<<16) | ((uint16_t)NVM.DATA1<<8) | NVM.DATA0;
        NVM_CMD = NVM_CMD_NO_OPERATION_gc;
        return crc; 
        //#]
    }
    
    //## operation readCalibrationByte(uint8_t)
    inline static uint8_t readCalibrationByte(uint8_t index) {
        //#[ operation readCalibrationByte(uint8_t)
        return NVM_readByte( NVM_CMD_READ_CALIB_ROW_gc, index );
        //#]
    }
    
    //## operation readDeviceRev()
    inline static uint8_t readDeviceRev();
    
    //## operation readFuseByte(uint8_t)
    inline static uint8_t readFuseByte(uint8_t index) {
        //#[ operation readFuseByte(uint8_t)
        waitWhileBusy();
        
        // Set address 
        NVM.ADDR0 = index;
        
        // Issue READ_FUSES command 
        executeCommand( NVM_CMD_READ_FUSES_gc );
        
        return NVM.DATA0;
        //#]
    }
    
    //## operation readUserSignature(uint16_t)
    inline static uint8_t readUserSignature(uint16_t index);
    
    //## operation readUserSignature(uint16_t,void *,uint16_t)
    static Stream::Status readUserSignature(uint16_t index, void * pData, uint16_t length);
    
    //## operation waitWhileBusy()
    inline static void waitWhileBusy() {
        //#[ operation waitWhileBusy()
        while ((NVM.STATUS & NVM_NVMBUSY_bm) == NVM_NVMBUSY_bm);
        //#]
    }
    
    //## operation writeUserSignature(uint16_t,void *,uint16_t)
    static void writeUserSignature(uint16_t index, void * pData, uint16_t length);

protected :

    //## operation eraseBootPage(uint32_t)
    inline static void eraseBootPage(uint32_t& address) {
        //#[ operation eraseBootPage(uint32_t)
        waitWhileBusy();
        NVM_commonSPM( address, NVM_CMD_ERASE_BOOT_PAGE_gc );  
        //#]
    }
    
    //## operation eraseWriteBootPage(uint32_t)
    inline static void eraseWriteBootPage(uint32_t& address) {
        //#[ operation eraseWriteBootPage(uint32_t)
        waitWhileBusy();
        NVM_commonSPM( address, NVM_CMD_ERASE_WRITE_BOOT_PAGE_gc );  
        //#]
    }
    
    // /**
    //  * \brief Flush flash page buffer
    //  *
    //  * Clear the NVM controller page buffer for flash. This needs to be called
    //  * before using \ref NVM_loadWordToFlashBuffer if it has not already been
    //  * cleared.
    //  *
    //  */
    //## operation flushBuffer()
    inline static void flushBuffer() {
        //#[ operation flushBuffer()
        waitWhileBusy();
        NVM_commonSPM( 0, NVM_CMD_ERASE_FLASH_BUFFER_gc );
        //#]
    }
    
    //## operation loadFlashPageBuffer(uint8_t)
    static void loadFlashPageBuffer(uint8_t* data);
    
    //## operation writeBootPage(uint32_t)
    inline static void writeBootPage(uint32_t& address) {
        //#[ operation writeBootPage(uint32_t)
        waitWhileBusy();
        NVM_commonSPM( address, NVM_CMD_WRITE_BOOT_PAGE_gc );  
        //#]
    }
};

inline void NvmController::eraseUserSignature() {
    //#[ operation eraseUserSignature()
    waitWhileBusy();
    NVM_commonSPM( 0, NVM_CMD_ERASE_USER_SIG_ROW_gc );   
    //#]
}

inline uint32_t NvmController::getMaxBooterSize() {
    //#[ operation getMaxBooterSize()
    return BOOT_SECTION_SIZE;
    //#]
}

inline uint16_t NvmController::getPageSize() {
    //#[ operation getPageSize()
    return APP_SECTION_PAGE_SIZE;
    //#]
}

inline uint8_t NvmController::readDeviceRev() {
    //#[ operation readDeviceRev()
    return MCU.REVID;
    //#]
}

inline uint8_t NvmController::readUserSignature(uint16_t index) {
    //#[ operation readUserSignature(uint16_t)
    return NVM_readByte( NVM_CMD_READ_USER_SIG_ROW_gc, index );
    //#]
}

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/NvmController.h
*********************************************************************/
