/*
 * Adc.cpp
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#include "Adc.h"
#include "Clock.h"

#ifdef ADCA
static uint8_t enableCountA( 0 );
#endif
#ifdef ADCB
static uint8_t enableCountB( 0 );
#endif

Adc& Adc::instance( uint8_t portNumber )
{
   switch ( portNumber )
   {
#ifdef ADCA
      case PortA:
         return *reinterpret_cast<Adc*>( &ADCA );
#endif

#ifdef ADCB
      case PortB:
         return *reinterpret_cast<Adc*>( &ADCB );
#endif
   }
   fatalErrorLoop();
   return *(Adc*) 0;
}

void Adc::disableClock()
{
#ifdef ADCA
   if ( this == reinterpret_cast<Adc*>( &ADCA ) )
   {
      if ( enableCountA )
      {
         enableCountA--;
         if ( !enableCountA )
         {
            Clock::disableModule( PortA, PR_ADC_bm );
         }
      }
   }
#endif

#ifdef ADCB
   if ( this == reinterpret_cast<Adc*>( &ADCB ) )
   {
      if ( enableCountB )
      {
         enableCountB--;
         if ( !enableCountB )
         {
            Clock::disableModule( PortB, PR_ADC_bm );
         }
      }
   }
#endif
}

void Adc::enableClock()
{
#ifdef ADCA
   if ( this == reinterpret_cast<Adc*>( &ADCA ) )
   {
      if ( enableCountA < 0xFF )
      {
         if ( !enableCountA )
         {
            Clock::enableModule( PortA, PR_ADC_bm );
         }
         enableCountA++;
      }
   }
#endif

#ifdef ADCB
   {
      if ( enableCountB < 0xFF )
      {
         if ( !enableCountB )
         {
            Clock::enableModule( PortB, PR_ADC_bm );
         }
         enableCountB++;
      }
   }
#endif
}

bool Adc::isEnabled()
{
   // It is sufficient to return the state of the ADC enable counters
   // since all driver functions that change the counts are protected
   // against interrupts and only the enable/disable functions leave the
   // counts incremented/decremented upon return.
#ifdef ADCA
   if ( this == reinterpret_cast<Adc*>( &ADCA ) )
   {
      return enableCountA;
   }
   else
#endif

#ifdef ADCB
   if ( this == reinterpret_cast<Adc*>( &ADCB ) )
   {
      return enableCountB;
   }
   else
#endif
   return false;
}
