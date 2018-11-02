/*
 * DigitalPortHw.h
 *
 *  Created on: 06.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_HwUnitBoards_DigitalPortHw_H
#define Electronics_HwUnitBoards_DigitalPortHw_H

#include "HwUnitBoards.h"

#include <Peripherals/IoPort.h>
#include <Peripherals/TimerCounter.h>

class DigitalPortHw
{
   ////    Constructors and destructors    ////

   public:

      DigitalPortHw( uint8_t portNumber );

      ////    Operations    ////

      void configure( uint8_t outputPins, uint8_t inputPins, uint8_t pwmPins = 0 );

      inline void off( uint8_t pin );

      void on( uint8_t pin, uint8_t value = 100 );

      ////    Additional operations    ////

      inline IoPort* getPort() const
      {
         return port;
      }

      inline void setPort( IoPort* p_port )
      {
         port = p_port;
      }

      inline TimerCounter* getTimerCounter0() const
      {
         return timerCounter0;
      }

      inline void setTimerCounter0( TimerCounter* p_timerCounter0 )
      {
         timerCounter0 = p_timerCounter0;
      }

      inline TimerCounter* getTimerCounter1() const
      {
         return timerCounter1;
      }

      inline void setTimerCounter1( TimerCounter* p_timerCounter1 )
      {
         timerCounter1 = p_timerCounter1;
      }

      ////    Attributes    ////

      IoPort* port;

      TimerCounter* timerCounter0;

      TimerCounter* timerCounter1;
};

inline void DigitalPortHw::off( uint8_t pin )
{
   on( pin, 0 );
}

#endif

