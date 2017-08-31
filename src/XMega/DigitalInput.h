/*
 * DigitalInput.h
 *
 *  Created on: 17.07.2017
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
         PortPin( _portPin.getPortNumber(), _portPin.getPinNumber() )
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
      uint8_t isSet() const;

      uint16_t getPulseWidth( bool level, uint16_t timeout ) const;
};

#endif

