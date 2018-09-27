/*
 * NvmLowLevel.S
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "NvmLowLevel.inc"

#ifndef __GNUC__
# error Unknown assembler
#endif                               

//! Value to write to CCP for access to protected IO registers.
#  define CCP_SPM_gc 0x9D

//! NVM command for loading flash buffer
#  define NVM_CMD_LOAD_FLASH_BUFFER_gc 0x23
     

PUBLIC_FUNCTION(NVM_readByte)

  lds r20, NVM_CMD          ; Store NVM command register
  mov ZL, r22               ; Load byte index into low byte of Z.
  mov ZH, r23               ; Load high byte into Z.
  sts NVM_CMD, r24          ; Load prepared command into NVM Command register.
  lpm r24, Z                ; Perform an LPM to read out byte
  sts NVM_CMD, r20          ; Restore NVM command register
  ret

END_FUNC(NVM_readByte)               


PUBLIC_FUNCTION_SEGMENT(NVM_commonSPM, BOOT)

	/**
	 * For GCC:
	 * \param address uint32_t r22:r25
	 * \param nvm_cmd uint8_t  r20
	 */
	in r25, RAMPZ         ; Store RAMPZ. Highest address byte is ignored, so using that
	out RAMPZ, r24        ; Load R24 into RAMPZ
	movw ZL, r22          ; Load R22:R23 into Z.
	lds r24, NVM_CMD      ; Store NVM command register (r24 is no longer needed)
	sts NVM_CMD, r20      ; Load prepared command into NVM Command register. 
	ldi r23, CCP_SPM_gc   ; Prepare Protect SPM signature (r23 is no longer needed)
	sts CCP, r23          ; Enable SPM operation (this disables interrupts for 4 cycles).
	spm                   ; Self-program.
	sts NVM_CMD, r24      ; Restore NVM command register
	out RAMPZ, r25        ; Restore RAMPZ register.
	ret

END_FUNC(NVM_commonSPM)


PUBLIC_FUNCTION_SEGMENT(NVM_loadWordToFlashBuffer, BOOT)

	/**
	 * For GCC:
	 * \param word_addr uint32_t r22:r25
	 * \param data      uint16_t r20:r21
	 */
wait_nvm:
	lds r18, NVM_STATUS
	sbrc r18, NVM_NVMBUSY_bp
	rjmp wait_nvm

	in r25, RAMPZ         ; Store RAMPZ. Highest address byte is ignored, so using that
	out RAMPZ, r24        ; Load R24 into RAMPZ
	movw ZL, r22          ; Load R22:R23 into Z.

	lds r24, NVM_CMD      ; Store NVM command register (r24 is no longer needed)
	ldi r18, NVM_CMD_LOAD_FLASH_BUFFER_gc
	sts NVM_CMD, r18      ; Load prepared command into NVM Command register.

	movw r0, r20          ; Load R20:R21 into R0:R1
	spm                   ; Self-program.

	clr r1                ; Clear R1 for GCC _zero_reg_ to function properly.
	sts NVM_CMD, r24      ; Restore NVM command register
	out RAMPZ, r25        ; Restore RAMPZ register.
	ret

END_FUNC(NVM_loadWordToFlashBuffer)

END_FILE()
