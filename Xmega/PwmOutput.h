/*
 * PwmOutput.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_PwmOutput_H
#define Basics_PwmOutput_H

#include "Basics.h"
#include "PortPin.h"
class TimerCounter0;

class TimerCounter1;


class PwmOutput : public PortPin
{
   ////    Constructors and destructors    ////

   public:

      inline PwmOutput( uint8_t _portNumber, uint8_t _pinNumber ) : PortPin( _portNumber, _pinNumber )
      {
      }

      ////    Operations    ////

      inline void clear();

      uint16_t isSet();

      void set( uint8_t value = 100 );
};

inline void PwmOutput::clear()
{
   set( 0 );
}

#endif
