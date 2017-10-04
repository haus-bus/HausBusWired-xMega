/*
 * NvmLowLevel.inc
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_NvmLowLevel_H
#define Peripherals_NvmLowLevel_H

#include "Assembler.inc"

#ifndef __ASSEMBLER__         

#ifdef __cplusplus
extern "C" {
#endif


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
uint8_t NVM_readByte(uint8_t command, uint16_t address);

#ifdef __cplusplus
}
#endif

#endif  // __ASSEMBLER__

#endif

