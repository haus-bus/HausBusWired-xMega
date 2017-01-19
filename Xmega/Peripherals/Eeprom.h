/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: Eeprom
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/Eeprom.h
*********************************************************************/

#ifndef Peripherals_Eeprom_H
#define Peripherals_Eeprom_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## dependency NvmController
#include "Peripherals/NvmController.h"
//## operation read(uint16_t,void *,uint16_t)
#include <Stream.h>
//## dependency GlobalInterrupt
class GlobalInterrupt;

//## package Peripherals

//## class Eeprom
class Eeprom {
public :

    //## class Eeprom::MemoryMapped
    class MemoryMapped {
        ////    Constructors and destructors    ////
        
    public :
    
        //## operation MemoryMapped()
        inline MemoryMapped() {
            //#[ operation MemoryMapped()
            NVM.CTRLB |= NVM_EEMAPEN_bm;
            //#]
        }
        
        //## operation ~MemoryMapped()
        inline ~MemoryMapped() {
            //#[ operation ~MemoryMapped()
            NVM.CTRLB &= ~NVM_EEMAPEN_bm;
            //#]
        }
    };
    
    ////    Operations    ////
    
    //## operation disablePowerReduction()
    inline static void disablePowerReduction() {
        //#[ operation disablePowerReduction()
        NVM.CTRLB &= ~NVM_EPRM_bm;
        //#]
    }
    
    //## operation enablePowerReduction()
    inline static void enablePowerReduction() {
        //#[ operation enablePowerReduction()
        NVM.CTRLB |= NVM_EPRM_bm;
        //#]
    }
    
    //## operation erase()
    inline static void erase();
    
    //## operation executeCommand(NVM_CMD_t)
    static void executeCommand(NVM_CMD_t command);
    
    //## operation read(uint16_t)
    static uint8_t read(uint16_t offset);
    
    //## operation read(uint16_t,void *,uint16_t)
    static Stream::Status read(uint16_t offset, void * pData, uint16_t length);
    
    //## operation write(uint16_t,uint8_t)
    static bool write(uint16_t offset, uint8_t value);
    
    //## operation write(uint16_t,void *,uint16_t)
    static Stream::Status write(uint16_t offset, void * pData, uint16_t length);

protected :

    //## operation fillBufferWithValue(uint8_t)
    static void fillBufferWithValue(uint8_t value);
    
    //## operation flushBuffer()
    inline static void flushBuffer();

private :

    //## operation isAddressInRange(uint16_t)
    static bool isAddressInRange(uint16_t address);
    
    ////    Attributes    ////

protected :

    static const uint8_t debugLevel;		//## attribute debugLevel
};

inline void Eeprom::erase() {
    //#[ operation erase()
    WARN_1( "ee.erase" );     
    fillBufferWithValue( 0xFF );
    NvmController::waitWhileBusy();
    executeCommand( NVM_CMD_ERASE_EEPROM_gc );
    //#]
}

inline void Eeprom::flushBuffer() {
    //#[ operation flushBuffer()
    NvmController::waitWhileBusy();
    
    // Flush EEPROM page buffer if necessary.
    if( (NVM.STATUS & NVM_EELOAD_bm) != 0) 
    {         
      DEBUG_M1( FSTR("flush") );                
    	executeCommand( NVM_CMD_ERASE_EEPROM_BUFFER_gc );
    }
    
    
    //#]
}

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/Eeprom.h
*********************************************************************/
