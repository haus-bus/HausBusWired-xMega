/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: NvmLowLevel
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/Assembler/NvmLowLevel/NvmLowLevel.h
*********************************************************************/

#ifndef Peripherals_Assembler_NvmLowLevel_NvmLowLevel_H
#define Peripherals_Assembler_NvmLowLevel_NvmLowLevel_H

//## auto_generated
#include <Assembler.h>
//## package Peripherals::Assembler::NvmLowLevel



//#[ ignore
#ifndef __ASSEMBLER__         

#ifdef __cplusplus
extern "C" {
#endif
//#]


// /**
//  * \brief Perform SPM write
//  * \internal
//  *
//  * This function sets the specified NVM_CMD, sets CCP and then runs the SPM
//  * instruction to write to flash.
//  *
//  * \note Interrupts should be disabled before running this function
//  *       if program memory/NVM controller is accessed from ISRs.
//  *
//  * \param address Address to perform the SPM on.
//  * \param command NVM command to use in the NVM_CMD register
//  */
//## operation NVM_commonSPM(uint32_t,uint8_t)
void NVM_commonSPM(uint32_t address, uint8_t command);

// /**
//  * \brief Load word into flash page buffer
//  *
//  * Clear the NVM controller page buffer for flash. This needs to be called
//  * before using \ref nvm_flash_load_word_to_buffer if it has not already been
//  * cleared.
//  *
//  * \param wordAddress Address to store data. The upper bits beyond the page size
//  *                                is ignored. \ref FLASH_PAGE_SIZE
//  * \param data Data word to load into the page buffer
//  */
//## operation NVM_loadWordToFlashBuffer(uint32_t,uint16_t)
void NVM_loadWordToFlashBuffer(uint32_t wordAddress, uint16_t data);

// /**
//  * \brief Read one byte using the LDI instruction
//  * \internal
//  *
//  * This function sets the specified NVM_CMD, reads one byte using at the
//  * specified byte address with the LPM instruction. NVM_CMD is restored after
//  * use.
//  *
//  * \note Interrupts should be disabled before running this function
//  *       if program memory/NVM controller is accessed from ISRs.
//  *
//  * \param command NVM command to load before running LPM
//  * \param address Byte offset into the signature row
//  */
//## operation NVM_readByte(uint8_t,uint16_t)
uint8_t NVM_readByte(uint8_t command, uint16_t address);


//#[ ignore
#ifdef __cplusplus
}
#endif

#endif  // __ASSEMBLER__
//#]

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/Assembler/NvmLowLevel/NvmLowLevel.h
*********************************************************************/
