/*
 * GlobalInterruptAVR.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_GlobalInterruptAVR_H
#define Basics_GlobalInterruptAVR_H

#include <avr/interrupt.h>

class GlobalInterrupt
{
   ////    Operations    ////

   public:

      typedef unsigned char sreg_t;

      inline static void disable()
      {
         cli();
      }

      inline static void enable()
      {
         sei();
      }

      inline static void waitForInterrupt()
      {
         __asm__ __volatile__ ( "sleep" );
      }

   protected:

      inline static sreg_t lock()
      {
         return SREG; disable();
      }

      inline static void restore( sreg_t context )
      {
         SREG = context;
      }
};

#endif

