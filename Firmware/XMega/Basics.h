/*
 * Basics.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Basics_Basics_H
#define Basics_Basics_H

#include <DefaultTypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>


// ----------------------------------------------------------
// Macros to access strings defined in PROGMEM above 64kB
// ----------------------------------------------------------
#define FAR( var )                                      \
   ( {                                                    \
      uint_farptr_t tmp;                                \
                                                      \
      __asm__ __volatile__ (                             \
                                                      \
         "ldi    %A0, lo8(%1)"           "\n\t"    \
         "ldi    %B0, hi8(%1)"           "\n\t"    \
         "ldi    %C0, hh8(%1)"           "\n\t"    \
         "clr    %D0"                    "\n\t"    \
         :                                             \
         "=d" ( tmp )                                \
         :                                             \
         "p"  ( &( var ) )                             \
         );                                                \
      tmp;                                              \
   } )
;

#ifdef __INTR_ATTRS     // default ISR attributes from avr/interrupt.h
# define        INTERRUPT       extern "C" __attribute__( ( __signal__, __INTR_ATTRS ) )
#else
# define        INTERRUPT       extern "C" __attribute__( ( __signal__ ) )
#endif

#define NO_RETURN       __attribute__( ( __noreturn__ ) )

#define STACK_POINTER() ( (char*)AVR_STACK_POINTER_REG )

#define CYCLES_PER_LOOP 8
#define _COUNT_DELAY_US( us, cycles ) ( (uint16_t)( ( (double)( F_CPU ) / ( (double)1000000 * (double)cycles ) ) * us + 0.5 ) )
#define COUNT_DELAY_BIT_US( us ) ( _COUNT_DELAY_US( us, CYCLES_PER_LOOP ) == 0 ? 1 : _COUNT_DELAY_US( us, CYCLES_PER_LOOP ) )

extern "C" void __cxa_pure_virtual();

inline static uint16_t _delayBitBreakOff( uint16_t count, volatile uint8_t* bit,
                                          uint8_t mask )
{
   asm volatile (
      "1:" "\n\t"
      "    ld __tmp_reg__, z" "\n\t"
      "    and __tmp_reg__, %[mask]" "\n\t"
      "    breq 2f" "\n\t"
      "    nop"   "\n\t"
      "    sbiw %[count],1" "\n\t"
      "    brne 1b" "\n\t"
      "2:" "\n\t"
      : [count] "+w" ( count )
      : [bit] "z" ( bit ), [mask] "r" ( mask ) );

   return count;
}

inline static uint16_t _delayBitBreakOn( uint16_t count, volatile uint8_t* bit,
                                         uint8_t mask )
{
   asm volatile (
      "1:" "\n\t"
      "    ld __tmp_reg__, z" "\n\t"
      "    and __tmp_reg__, %[mask]" "\n\t"
      "    brne 2f" "\n\t"
      "    nop"   "\n\t"
      "    sbiw %[count],1" "\n\t"
      "    brne 1b" "\n\t"
      "2:" "\n\t"
      : [count] "+w" ( count )
      : [bit] "z" ( bit ), [mask] "r" ( mask ) );

   return count;
}

inline static uint16_t delayBit( uint16_t count, volatile uint8_t* bit,
                                 uint8_t mask, uint8_t breakOn )
{
   if ( breakOn )
   {
      return _delayBitBreakOn( count, bit, mask );
   }
   else
   {
      return _delayBitBreakOff( count, bit, mask );
   }
}

bool isRunning();

#endif
