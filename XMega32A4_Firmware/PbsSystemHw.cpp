/*
 * MS6SystemHw.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "PbsSystemHw.h"
#include <Release.h>
#include <Security/Checksum.h>
#include <HomeAutomationConfiguration.h>
#include <Traces/Logger.h>
#include <LogicalButton.h>
#include <Security/ModuleId.h>
#include <PersistentRules.h>
#include <Peripherals/TimerCounter.h>
#include <Gateway.h>
#include <SoftTwi.h>
#include <RS485Hw.h>


const ModuleId moduleId = { "$MOD$ PBS      ",
                            0,
                            Release::MAJOR,
                            Release::MINOR,
                            CONTROLLER_ID,
                            0 };

PbsSystemHw::PbsSystemHw()
{
   configure();
}

#ifdef _DEBUG_
void putc( char c )
{
   Usart::instance<DBG_PORT, DBG_CHANNEL>().write( c );
}
#endif

void PbsSystemHw::configure()
{
#ifdef _DEBUG_
   // configure Logger
   Usart::instance<DBG_PORT, DBG_CHANNEL>().init<DBG_BAUDRATE>();
   Logger::instance().setStream( putc );
#endif // _DEBUG_

   // configure ports
   TRACE_PORT_INIT( AllPins );

   DEBUG_H1( FSTR( "configure" ) );
   if ( CONTROLLER_ID == SD485_ID )
   {
      configureRs485();
   }
   else
   {
      IoPort::instance( PortE ).configure( Pin0 | Pin1 | Pin2, PORT_OPC_PULLUP_gc );
      IoPort::instance( PortE ).setPinsAsOutput( Pin3 );
      IoPort::instance( PortE ).setPins( Pin3 );

      IoPort::instance( PortR ).setPinsAsOutput( Pin0 | Pin1 );
      IoPort::instance( PortR ).setPins( Pin0 | Pin1 );
      configureTwi();
   }

   // enable interrupts
   enableInterrupts();
}

void PbsSystemHw::configureLogicalButtons()
{
   DEBUG_M1( FSTR( "LButtons" ) );

   uint8_t i = 0;
   uint8_t mask = HomeAutomationConfiguration::instance().getLogicalButtonMask();
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

#ifdef SUPPORT_RS485

RS485Hw rs485Hw( Usart::instance<PortE, 0>(), DigitalOutput( PortDummy, 0 ), DigitalOutput( PortR, 0 ) );

INTERRUPT void USARTE0_RXC_vect()
{
   rs485Hw.handleDataReceived();
}

void PbsSystemHw::configureRs485()
{
   DEBUG_M1( FSTR( "RS485" ) );
   Usart::configPortPins<PortE, 0>();
   new Gateway( &rs485Hw, Gateway::RS485 );
}
#endif

#ifdef SUPPORT_TWI

SoftTwi twi;

void PbsSystemHw::configureTwi()
{
   new Gateway( &twi, Gateway::TWI );
}

#endif