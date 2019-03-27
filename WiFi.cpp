/*
 * WiFi.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include "WiFi.h"
#include <DigitalOutput.h>
#include <FlashString.h>

#include <util/delay.h>

WiFi::WiFi( Usart& _usart, DigitalOutput _resetPin ) : usart( &_usart ), resetPin( _resetPin )
{
}

uint8_t WiFi::enterCommandMode( bool sync )
{
   if ( resetPin.isSet() )
   {
      resetPin.clear();
      if ( !sync )
      {
         return MIN_RESET_TIME + 50;
      }
      _delay_ms( MIN_RESET_TIME + 50 );
   }
   resetPin.set();
   return 0;
}

void WiFi::exitCommandMode()
{
   usart->write( FSTR( "at+escape=1\r\n" ), 10 );
}

uint16_t WiFi::resetToFactory( bool sync )
{
   if ( resetPin.isSet() )
   {
      resetPin.getIoPort().setPinsAsOutput( resetPin.getPin() );
      resetPin.clear();
      if ( !sync )
      {
         return MIN_FACTORY_RESET_TIME + 500;
      }
      _delay_ms( MIN_FACTORY_RESET_TIME + 500 );
   }
   resetPin.getIoPort().setPinsAsInput( resetPin.getPin() );
   resetPin.set();
   return 0;
}