/*
 * GlobalInterrupt.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_GlobalInterrupt_H
#define Basics_GlobalInterrupt_H

#include <avr/interrupt.h>

class GlobalInterrupt
{
   ////    Operations    ////

   public:

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
};

#endif

