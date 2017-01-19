/*
 * IrSupport.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "IrSupport.h"

#include <PortPin.h>
#include <Peripherals/EventSystem.h>
#include <Peripherals/TimerCounter0.h>

static IrDecoder* irDecoder( 0 );

INTERRUPT void TCE0_CCA_vect()
{
  TRACE_PORT_SET( TR_INT_PIN );
#ifndef _TRACE_PORT_
  notifyBusy();
#endif
  static uint16_t lastStamp = 0;

  // save pulse time
  uint16_t stamp = TCE0.CCA;

  // get edge
  bool edge = ((stamp & 0x8000) ? true : false);

  // clear edge information
  stamp &= ~0x8000;

  if ( irDecoder )
  {
    irDecoder->notifyEdge( edge, (stamp - lastStamp) & 0x7FFF );
  }
  lastStamp = stamp;

  TRACE_PORT_CLEAR( TR_INT_PIN );
}

void configureInfraRedHw( PortPin portPin, IrDecoder* decoder )
{
  DEBUG_M1( FSTR("IR") );
  if ( !irDecoder )
  {
    irDecoder = decoder;

    EventSystem::setEventSource(
        2,
        (EVSYS_CHMUX_t) (EVSYS_CHMUX_PORTA_PIN0_gc
            + (portPin.getPortNumber() << 3) + portPin.getPinNumber()) );

    TimerCounter0& t = TimerCounter0::instance( PortE );
    t.configWGM( TC_WGMODE_NORMAL_gc );
    t.setPeriod( 0x7FFF );
#if F_CPU == 32000000UL
    t.configClockSource( TC_CLKSEL_DIV1024_gc );
#elif F_CPU == 8000000UL
    t.configClockSource( TC_CLKSEL_DIV256_gc );
#else
#    error "F_CPU is not supported for IRHardware"
#endif

    t.configInputCapture( TC_EVSEL_CH2_gc );
    t.enableChannels( TC0_CCAEN_bm );
    t.setCCAIntLevel( TC_CCAINTLVL_MED_gc );
  }

}
