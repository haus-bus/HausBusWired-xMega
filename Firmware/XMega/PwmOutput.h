/*
 * PwmOutput.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_PwmOutput_H
#define Basics_PwmOutput_H

#include "PortPin.h"

class TimerCounter0;

class TimerCounter1;

class PwmOutput : public PortPin
{
   ////    Constructors and destructors    ////

   public:

      PwmOutput( uint8_t _portNumber, uint8_t _pinNumber, uint16_t _period = 100 );

      ////    Operations    ////

      inline void clear()
      {
         set( 0 );
      }

      uint16_t isSet();

      void set( uint8_t value = 100 );

      void setPeriode( uint16_t period );

   protected:

      TimerCounter0* getTC0();
      TimerCounter1* getTC1();

};

#endif

