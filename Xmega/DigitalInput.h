/*
 * DigitalInput.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Basics_DigitalInput_H
#define Basics_DigitalInput_H

#include "PortPin.h"

class DigitalInput : public PortPin
{
   ////    Constructors and destructors    ////

   public:

      inline DigitalInput( PortPin _portPin ) :
         PortPin( _portPin )
      {
         if ( isValid() )
         {
            configInput();
         }
      }

      inline DigitalInput( uint8_t _portNumber, uint8_t _pinNumber ) :
         PortPin( _portNumber, _pinNumber )
      {
         if ( isValid() )
         {
            configInput();
         }
      }

      ////    Operations    ////

};

template<uint8_t portNumber, uint8_t pinNumber>
class DigitalInputTmpl : public PortPinTmpl<portNumber, pinNumber>
{
   ////    Constructors and destructors    ////

   public:

      inline DigitalInputTmpl()
      {
         if ( this->isValid() )
         {
            this->configInput();
         }
      }

      ////    Operations    ////

};

#endif

