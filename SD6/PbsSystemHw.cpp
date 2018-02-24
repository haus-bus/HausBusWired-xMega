/*
 * MS6SystemHw.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "PbsSystemHw.h"
#include <Release.h>
#include <Security/Checksum.h>
#include <HwConfiguration.h>
#include <Traces/Logger.h>
#include <LogicalButton.h>
#include <Security/ModuleId.h>
#include <PersistentRules.h>
#include <Peripherals/TimerCounter0.h>
#include <Gateway.h>
#include <SoftTwi.h>

MOD_ID_SECTION const ModuleId moduleId = { "$MOD$ PBS      ", 0, Release::MAJOR,
                                           Release::MINOR, CONTROLLER_ID, 0 };

PbsSystemHw::PbsSystemHw()
{
   configure();
}

#ifdef _DEBUG_
void putc( char c )
{
   Usart::instance( PortE, 0 ).write( c );
}
#endif

void PbsSystemHw::configure()
{
#ifdef _DEBUG_
   // configure Logger
   Usart::instance( PortE, 0 ).init<BAUDRATE,   // baudrate
                                    true,       // doubleClock
                                    USART_CMODE_ASYNCHRONOUS_gc, // asynchronous communication
                                    USART_PMODE_DISABLED_gc, // NoParity
                                    USART_CHSIZE_8BIT_gc, // 8-Bits
                                    false       // 1 stopBit
                                    >();
   Logger::instance().setStream( putc );
#endif // _DEBUG_

   HwConfiguration::storage = &internalEeprom;

   // configure ports
   TRACE_PORT_INIT( AllPins );

   IoPort::instance( PortE ).configure( Pin0 | Pin1 | Pin2, PORT_OPC_PULLUP_gc );
   IoPort::instance( PortE ).setPinsAsOutput( Pin3 );
   IoPort::instance( PortE ).setPins( Pin3 );

   IoPort::instance( PortR ).setPinsAsOutput( Pin0 | Pin1 );
   IoPort::instance( PortR ).setPins( Pin0 | Pin1 );

   DEBUG_H1( FSTR( "configure" ) );

   // configureOneWire();
   configureLogicalButtons();

   // enable interrupts
   enableInterrupts();
}

void PbsSystemHw::configureLogicalButtons()
{
   DEBUG_M1( FSTR( "LButtons" ) );

   uint8_t i = 0;
   uint8_t mask = HwConfiguration::HomeAutomation::instance().getLogicalButtonMask();
   while ( mask )
   {
      if ( mask & 1 )
      {
         new LogicalButton( i + 1 );
      }
      mask >>= 1;
      i++;
   }
}

void PbsSystemHw::configureTwi()
{
   static SoftTwi twi;
   new Gateway( &twi, Gateway::TWI );
}

