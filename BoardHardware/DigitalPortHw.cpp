/*
 * DigitalPortHw.cpp
 *
 *  Created on: 06.08.2014
 *      Author: Viktor Pankraz
 */

#include "DigitalPortHw.h"


DigitalPortHw::DigitalPortHw( uint8_t portNumber ) : port( &IoPort::instance( portNumber ) ), timerCounter0( &TimerCounter::instance( portNumber, 0 ) ), timerCounter1( &TimerCounter::instance( portNumber, 1 ) )
{
}

void DigitalPortHw::configure( uint8_t outputPins, uint8_t inputPins, uint8_t pwmPins )
{
   port->setPinsAsOutput( outputPins | pwmPins );
   port->clearPins( outputPins | pwmPins );
   port->setPinsAsInput( inputPins );
   if ( inputPins )
   {
      port->configure( inputPins,
                       PORT_OPC_PULLUP_gc,      // enable pullup
                       false,                   // do not invert
                       PORT_ISC_BOTHEDGES_gc,   // sense on both edges
                       true );                  // enable slewRate
   }

   if ( timerCounter0 )
   {
#if F_CPU == 32000000UL
      timerCounter0->configClockSource( TC_CLKSEL_DIV8_gc );
#elif F_CPU == 8000000UL
      timerCounter0->configClockSource( TC_CLKSEL_DIV1_gc );
#else
#    error "F_CPU is not supported for PwmHardware"
#endif

      timerCounter0->configWGM( TC_WGMODE_SS_gc );
      timerCounter0->setPeriod( 100 );
      timerCounter0->enableChannels( pwmPins << 4 );
   }
   if ( timerCounter1 )
   {
#if F_CPU == 32000000UL
      timerCounter1->configClockSource( TC_CLKSEL_DIV8_gc );
#elif F_CPU == 8000000UL
      timerCounter1->configClockSource( TC_CLKSEL_DIV1_gc );
#else
#    error "F_CPU is not supported for PwmHardware"
#endif
      timerCounter1->configWGM( TC_WGMODE_SS_gc );
      timerCounter1->setPeriod( 100 );
      timerCounter1->enableChannels( pwmPins );
   }
}

void DigitalPortHw::on( uint8_t pin, uint8_t value )
{
   if ( ( pin < Pin4 ) && timerCounter0 )
   {
      if ( pin == Pin0 )
      {
         timerCounter0->setCompareA( value );
      }
      else if ( pin == Pin1 )
      {
         timerCounter0->setCompareB( value );
      }
      else if ( pin == Pin2 )
      {
         timerCounter0->setCompareC( value );
      }
      else if ( pin == Pin3 )
      {
         timerCounter0->setCompareD( value );
      }
   }
   else if ( ( pin < Pin6 ) && timerCounter1 )
   {
      if ( pin == Pin4 )
      {
         timerCounter1->setCompareA( value );
      }
      else if ( pin == Pin5 )
      {
         timerCounter1->setCompareB( value );
      }
   }
   else
   {
      if ( value )
      {
         port->setPins( pin );
      }
      else
      {
         port->clearPins( pin );
      }
   }
}