/*
 * PwmOutput.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include "PwmOutput.h"
#include "Peripherals/TimerCounter0.h"
#include "Peripherals/TimerCounter1.h"

uint16_t PwmOutput::isSet()
{
   TimerCounter0* tc0 = &TimerCounter0::instance( portNumber );
   if ( tc0 && ( pinNumber < Pin4 ) )
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

   TimerCounter1* tc1 = &TimerCounter1::instance( portNumber );
   if ( tc1 && ( pinNumber >= Pin4 ) )
   {
      if ( pinNumber == 4 )
      {
         tc1->getCaptureA();
      }
      else if ( pinNumber == 5 )
      {
         tc1->getCaptureB();
      }
   }
   return 0;
}

void PwmOutput::set( uint8_t value )
{
   TimerCounter0* tc0 = &TimerCounter0::instance( portNumber );
   if ( tc0 && ( pinNumber < Pin4 ) )
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
      return;
   }

   TimerCounter1* tc1 = &TimerCounter1::instance( portNumber );
   if ( tc1 && ( pinNumber >= Pin4 ) )
   {
      if ( pinNumber == 4 )
      {
         tc1->setCompareA( value );
      }
      else if ( pinNumber == 5 )
      {
         tc1->setCompareB( value );
      }
   }

}