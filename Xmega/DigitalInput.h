/*
 * DigitalInput.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Basics_DigitalInput_H
#define Basics_DigitalInput_H

#include "Basics.h"
#include "PortPin.h"

class DigitalInput : public PortPin
{
   ////    Constructors and destructors    ////

   public:

      inline DigitalInput( uint8_t _portNumber, uint8_t _pinNumber ) :
         PortPin( _portNumber, _pinNumber )
      {

      }

      ////    Operations    ////
      uint8_t isSet() const;

      uint16_t getPulseWidth( bool level, uint16_t timeout ) const;
};

#endif

