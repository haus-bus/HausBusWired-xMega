/*
 * NonCriticalSection.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_NonCriticalSection_H
#define Basics_NonCriticalSection_H

#include "Basics.h"

#include "GlobalInterrupt.h"

class NonCriticalSection : public GlobalInterrupt
{
   ////    Constructors and destructors    ////

   public:

      inline NonCriticalSection()
      {
         sreg = SREG;
         enable();
      }

      inline ~NonCriticalSection()
      {
         SREG = sreg;
      }

      ////    Attributes    ////

   private:

      unsigned char sreg;       // ## attribute sreg
};

#endif

