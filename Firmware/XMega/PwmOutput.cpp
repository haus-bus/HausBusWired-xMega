/*
 * PwmOutput.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include <stddef.h>
#include "PwmOutput.h"
#include "Peripherals/TimerCounter0.h"
#include "Peripherals/TimerCounter1.h"


PwmOutput::PwmOutput( uint8_t _portNumber, uint8_t _pinNumber, uint16_t _period ) : PortPin( _portNumber, _pinNumber )
{
   configOutput();
   TimerCounter0* tc0 = getTC0();
   TimerCounter1* tc1 = getTC1();
   if ( tc0 )
   {
      tc0->configClockSource( TC_CLKSEL_DIV8_gc );
      tc0->configWGM( TC_WGMODE_SS_gc );
      tc0->setPeriod( _period );
      tc0->enableChannels( 1 << ( _pinNumber + 4 ) );
   }
   else if ( tc1 )
   {
      tc1->configClockSource( TC_CLKSEL_DIV8_gc );
      tc1->configWGM( TC_WGMODE_SS_gc );
      tc1->setPeriod( _period );
      tc1->enableChannels( 1 << _pinNumber );
   }
}

uint16_t PwmOutput::isSet()
{
   TimerCounter0* tc0 = getTC0();
   TimerCounter1* tc1 = getTC1();
   if ( tc0 )
   {
      if ( pinNumber == 0 )
      {
         return tc0->getCaptureA();
      }
      else if ( pinNumber == 1 )
      {
         return tc0->getCaptureB();
      }
      else if ( pinNumber == 2 )
      {
         return tc0->getCaptureC();
      }
      else
      {
         return tc0->getCaptureD();
      }
   }
   else if ( tc1 )
   {
      if ( pinNumber == 4 )
      {
         tc1->getCaptureA();
      }
      else
      {
         tc1->getCaptureB();
      }
   }
   return 0;
}

void PwmOutput::set( uint8_t value )
{
   TimerCounter0* tc0 = getTC0();
   TimerCounter1* tc1 = getTC1();

   if ( tc0 )
   {
      if ( pinNumber == 0 )
      {
         tc0->setCompareA( value );
      }
      else if ( pinNumber == 1 )
      {
         tc0->setCompareB( value );
      }
      else if ( pinNumber == 2 )
      {
         tc0->setCompareC( value );
      }
      else
      {
         tc0->setCompareD( value );
      }
   }
   else if ( tc1 )
   {
      if ( pinNumber == 4 )
      {
         tc1->setCompareA( value );
      }
      else
      {
         tc1->setCompareB( value );
      }
   }
}

TimerCounter0* PwmOutput::getTC0()
{
   if ( pinNumber < 4 )
   {
      return &TimerCounter0::instance( portNumber );
   }
   return NULL;
}

TimerCounter1* PwmOutput::getTC1()
{
   if ( ( pinNumber >= 4 ) && ( pinNumber < 6 ) )
   {
      return &TimerCounter1::instance( portNumber );
   }
   return NULL;
}
