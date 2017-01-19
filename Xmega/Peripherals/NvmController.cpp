/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: NvmController
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/NvmController.cpp
*********************************************************************/

//## auto_generated
#include "Peripherals/NvmController.h"
//## package Peripherals

//## class NvmController
void NvmController::executeCommand(NVM_CMD_t command) {
    //#[ operation executeCommand(NVM_CMD_t)
    uint8_t oldCmd = NVM.CMD; 
    NVM.CMD = command;
    CCPWrite(&NVM.CTRLA, NVM_CMDEX_bm);   
    NVM.CMD = oldCmd;
    //#]
}

Stream::Status NvmController::readUserSignature(uint16_t index, void * pData, uint16_t length) {
    //#[ operation readUserSignature(uint16_t,void *,uint16_t)
    if( (index+length) > getPageSize() )
    {
      return Stream::ABORTED;
    }
    
    uint8_t* destination = static_cast<uint8_t*>(pData);
    
    while( length-- )
    {
      *destination++ = readUserSignature( index++ ); 
    }
    return Stream::SUCCESS;
    //#]
}

void NvmController::writeUserSignature(uint16_t index, void * pData, uint16_t length) {
    //#[ operation writeUserSignature(uint16_t,void *,uint16_t)
    if( (index+length) > getPageSize() )
    {
      return;
    }
    
    uint8_t buffer[getPageSize()];
    readUserSignature(0, buffer , sizeof(buffer) ); 
    memcpy( &buffer[index], pData, length );
    loadFlashPageBuffer( buffer ); 
    eraseUserSignature();
    waitWhileBusy();
    NVM_commonSPM( 0, NVM_CMD_WRITE_USER_SIG_ROW_gc );  
    //#]
}

void NvmController::loadFlashPageBuffer(uint8_t* data) {
    //#[ operation loadFlashPageBuffer(uint8_t)
    flushBuffer();
    for( uint16_t i=0; i<getPageSize(); i+=2 )
    {
      // Set up little-endian word.
      uint16_t w = *data++;
      w += (*data++) << 8;
      NVM_loadWordToFlashBuffer( i, w );
    }
    //#]
}

/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/NvmController.cpp
*********************************************************************/
