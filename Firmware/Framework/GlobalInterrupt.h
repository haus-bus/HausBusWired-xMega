/*
 * GlobalInterrupt.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_GlobalInterrupt_H
#define Basics_GlobalInterrupt_H

#ifdef __AVR__
#include <GlobalInterruptAVR.h>
#elif defined ESP8266
#include <GlobalInterruptESP8266.h>
#elif defined __DUMMY__

class GlobalInterrupt
{
   ////    Operations    ////

   public:

      typedef unsigned char sreg_t;

      inline static sreg_t lock()
      {
         return 0;
      }

      inline static void restore( sreg_t context )
      {
      }

      inline static void enable()
      {
      }

      inline static void disable()
      {
      }

      inline static void waitForInterrupt()
      {
      }


};

#else
#error "Unkown environment, please extend support here."
#endif
#endif
