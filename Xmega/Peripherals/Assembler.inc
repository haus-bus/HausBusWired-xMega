/*
 * Assembler.inc
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Assembler_Assembler_H
#define Peripherals_Assembler_Assembler_H

//## dependency io
#include <avr/io.h>
//## package Peripherals::Assembler



#ifdef __ASSEMBLER__

//#[ type MACROS
#ifndef __DOXYGEN__

	/* IAR doesn't accept dots in macro names */
	.macro  ld_addr, reg, sym
	lda.w   \reg, \sym
	.endm

	/* Define a function \a name that is either globally visible or only
	 * file-local.
	 */
	.macro gas_begin_func name, is_public
		.if \is_public
		.global \name
		.endif
		.section .text.\name, "ax", @progbits
		.type \name, @function
	\name :
	.endm

	/* Define a function \a name that is either globally visible or only
	 * file-local in a given segment.
	 */
	.macro gas_begin_func_segm name, is_public, segment
		.if \is_public
		.global \name
		.endif
		.section .\segment, "ax", @progbits
		.type \name, @function
	\name :
	.endm

	/* Define \a name as a weak alias for the function \a strong_name */
	.macro gas_weak_function_alias name, strong_name
	.global \name
	.weak   \name
	.type   \name, @function
	.set    \name, \strong_name
	.endm

	/* Define a weak function called \a name */
	.macro gas_weak_function name
	.weak   \name
	gas_begin_func \name 1
	.endm

#define REPEAT(count)           .rept   count
#define END_REPEAT()            .endr
#define FILL_BYTES(count)       .fill   count
#define SET_LOC(offset)         .org    offset
#define L(name)                 .L##name
#define EXTERN_SYMBOL(name)

#define TEXT_SECTION(name)                              \
	.section name, "ax", @progbits
#define RODATA_SECTION(name)                            \
	.section name, "a", @progbits
#define DATA_SECTION(name)                              \
	.section name, "aw", @progbits
#define BSS_SECTION(name)                               \
	.section name, "aw", @nobits

#define FUNCTION(name) gas_begin_func name 0
#define PUBLIC_FUNCTION(name)   gas_begin_func name 1
#define PUBLIC_FUNCTION_SEGMENT(name, segment)          \
	gas_begin_func_segm name 1 segment
#define WEAK_FUNCTION(name) gas_weak_function name
#define WEAK_FUNCTION_ALIAS(name, strong_name) \
	gas_weak_function_alias name strong_name
#define END_FUNC(name)                                  \
	.size   name, . - name

#define END_FILE()

#endif /* __DOXYGEN__ */
//#]

#endif

#endif

