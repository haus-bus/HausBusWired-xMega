/*
 * DigitalOutput.h
 *
 * Created: 18.06.2014 14:12:56
 * Author: viktor.pankraz
 */

#ifndef Basics_DigitalOutput_H
#define Basics_DigitalOutput_H

#include "Basics.h"
#include "DigitalInput.h"

class DigitalOutput : public DigitalInput
{
   ////    Constructors and destructors    ////

   public:

      inline DigitalOutput( uint8_t _portNumber, uint8_t _pinNumber ) : DigitalInput( _portNumber, _pinNumber )
      {
         // getIoPort().setPinsAsOutput( getPin() );
      }

      ////    Operations    ////

      void clear();

      void set();

      void toggle();
};

#endif
