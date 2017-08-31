/*
 * DigitalOutput.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_DigitalOutput_H
#define Basics_DigitalOutput_H

#include "DigitalInput.h"

class DigitalOutput : public DigitalInput
{
   ////    Constructors and destructors    ////

   public:

      inline DigitalOutput( uint8_t _portNumber, uint8_t _pinNumber ) : DigitalInput( _portNumber, _pinNumber )
      {
         if ( isValid() )
         {
            configOutput();
         }
      }

      inline DigitalOutput( PortPin _portPin ) : DigitalInput( _portPin )
      {
         if ( isValid() )
         {
            configOutput();
         }
      }

      ////    Operations    ////

      void clear();

      void set();

      void toggle();
};

#endif

