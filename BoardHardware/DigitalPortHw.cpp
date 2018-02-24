/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: AR8
        Configuration   : release
        Model Element	: DigitalPortHw
   //!	Generated Date	: Wed, 6, Aug 2014
        File Path	: AR8/release/Electronics/HwUnitBoards/DigitalPortHw.cpp
 *********************************************************************/

// ## auto_generated
#include "DigitalPortHw.h"
// ## package Electronics::HwUnitBoards

// ## class DigitalPortHw
DigitalPortHw::DigitalPortHw( uint8_t portNumber ) : port( IoPort::instance( portNumber ) ), timerCounter0( &TimerCounter0::instance( portNumber ) ), timerCounter1( &TimerCounter1::instance( portNumber ) )
{
   // #[ operation DigitalPortHw(uint8_t)
   // #]
}

void DigitalPortHw::configure( uint8_t outputPins, uint8_t inputPins, uint8_t pwmPins )
{
   // #[ operation configure(uint8_t,uint8_t,uint8_t)
   port.setPinsAsOutput( outputPins | pwmPins );
   port.clearPins( outputPins | pwmPins );
   port.setPinsAsInput( inputPins );
   if ( inputPins )
   {
      port.configure( inputPins,
                      PORT_OPC_PULLUP_gc,       // enable pullup
                      false,                    // do not invert
                      PORT_ISC_BOTHEDGES_gc,    // sense on both edges
                      true );                   // enable slewRate
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
   // #]
}

void DigitalPortHw::on( uint8_t pin, uint8_t value )
{
   // #[ operation on(uint8_t,uint8_t)
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
         port.setPins( pin );
      }
      else
      {
         port.clearPins( pin );
      }
   }
   // #]
}

/*********************************************************************
        File Path	: AR8/release/Electronics/HwUnitBoards/DigitalPortHw.cpp
*********************************************************************/
