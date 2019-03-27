/*
 * AnalogOutput.h
 *
 * Created: 18.06.2014 14:12:56
 * Author: viktor.pankraz
 */

#ifndef Basics_AnalogOutput_H
#define Basics_AnalogOutput_H

#include "Basics.h"
#include "AnalogInput.h"

class AnalogOutput : public AnalogInput
{
   ////    Constructors and destructors    ////

   public:

      inline AnalogOutput( uint8_t _portNumber, uint8_t _pinNumber ) : AnalogInput( portNumber, pinNumber )
      {
      }

      ////    Operations    ////

      inline void clear()
      {
         setValue( 0 );
         getIoPort().clearPins( getPin() );
      }

      inline bool isSet()
      {
         return value;
      }

      inline void set( uint16_t _value = MAX_VALUE )
      {
         setValue( _value );
         value ? getIoPort().setPins( getPin() ) : getIoPort().clearPins( getPin() );
      }
};

#endif
