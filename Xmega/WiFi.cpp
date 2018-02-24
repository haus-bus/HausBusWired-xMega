/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: AR8
        Configuration   : release
        Model Element	: WiFi
   //!	Generated Date	: Wed, 6, Aug 2014
        File Path	: AR8/release/Electronics/IntegratedCircuits/WiFi.cpp
 *********************************************************************/

// ## auto_generated
#include "WiFi.h"
// ## operation WiFi(Usart,DigitalOutput)
#include <DigitalOutput.h>
// ## dependency FlashString
#include <FlashString.h>
// ## package Electronics::IntegratedCircuits

// ## class WiFi
WiFi::WiFi( Usart& _usart, DigitalOutput _resetPin ) : usart( _usart ), resetPin( _resetPin )
{
   // #[ operation WiFi(Usart,DigitalOutput)
   // #]
}

uint8_t WiFi::enterCommandMode( bool sync )
{
   // #[ operation enterCommandMode(bool)
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
   // #]
}

void WiFi::exitCommandMode()
{
   // #[ operation exitCommandMode()
   usart.write( FSTR( "at+escape=1\r\n" ), 10 );
   // #]
}

uint16_t WiFi::resetToFactory( bool sync )
{
   // #[ operation resetToFactory(bool)
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
   // #]
}

/*********************************************************************
        File Path	: AR8/release/Electronics/IntegratedCircuits/WiFi.cpp
*********************************************************************/
