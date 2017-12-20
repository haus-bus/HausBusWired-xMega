/*
 * HmwBooterStreamHw.cpp
 *
 * Created: 23.11.2017 19:23:55
 * Author: viktor.pankraz
 */


#include "HmwBooterHw.h"
#include <Tracing/Logger.h>
#include <Time/Timestamp.h>
#include <DigitalOutput.h>
#include <Release.h>

#if ( HARDWARE_ID == 0 )

static DigitalOutputTmpl<PortE, 0> rxEnable;
static DigitalOutputTmpl<PortE, 1> txEnable;
static DigitalInputTmpl<PortE, 2> rx;
static DigitalOutputTmpl<PortE, 3> tx;

#elif ( HARDWARE_ID == 1 )

static DigitalOutputTmpl<PortE, 1> txEnable;
static DigitalInputTmpl<PortE, 2> rx;
static DigitalOutputTmpl<PortE, 3> tx;

#else
#error "Hardware not supported!!!"
#endif

static const uint8_t LED_MASK = 0x3F;
static uint8_t ledData[] = { 0x00, 0x01, 0x05, 0x15, 0x35, 0x3D, 0x3F, 0x3E, 0x3A, 0x2A, 0x0A, 0x02 };
static uint8_t ledIdx = 0;


static Timestamp lastTime;

HmwBooterHw::HmwBooterHw()
{
   serial = &Usart::instance<PortE, 0>();
   PORTC.DIR = LED_MASK;

#ifdef DEBUG
   DigitalInputTmpl< PortC, 6 > rx;
   DigitalOutputTmpl<PortC, 7> tx;
   Usart::instance<PortC, 1>().init<115200>();
   Logger::instance().setStream( HmwBooterHw::debug );
#endif
}

void HmwBooterHw::enableTranceiver( bool enable )
{
   enable ? txEnable.set() : txEnable.clear();
}

void HmwBooterHw::debug( char c )
{
   Usart::instance<PortC, 1>().write( c );
}

void HmwBooterHw::notifyNextDownloadPacket()
{
   if ( ++ledIdx >= sizeof( ledData ) )
   {
      ledIdx = 0;
   }
}



void HmwBooterHw::handleLeds( bool isDownloadRunning )
{
   if ( isDownloadRunning )
   {
      uint8_t otherPins = PORTC.OUT & ~LED_MASK;
      PORTC.OUT = ledData[ledIdx] | otherPins;
   }
   else
   {
      if ( PORTC.IN & LED_MASK )
      {
         if ( lastTime.since() > 990 )
         {
            PORTC.OUTTGL = LED_MASK;
            lastTime = Timestamp();
         }
      }
      else
      {
         if ( lastTime.since() > 10 )
         {
            PORTC.OUTTGL = LED_MASK;
            lastTime = Timestamp();
         }
      }
   }
}