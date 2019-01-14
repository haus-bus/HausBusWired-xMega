/*
 * AR8SystemHw.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include "AR8SystemHw.h"

#include <Security/Checksum.h>
#include <Dimmer.h>
#include <DS1820.h>
#include <Peripherals/Eeprom.h>
#include <Peripherals/EventSystem.h>
#include <HacfIpStackManager.h>
#include <IrReceiver.h>
#include <Traces/Logger.h>
#include <LogicalButton.h>
#include <DigitalOutputUnit.h>
#include <Security/ModuleId.h>
#include <PersistentRules.h>
#include <RollerShutter.h>
#include <Peripherals/TimerCounter.h>
#include <Protocols/IpStack/UdpConnection.h>
#include <Protocols/Ethernet/MAC.h>
#include <Gateway.h>
#include <HacfIpStackManager.h>
#include <HomeAutomationConfiguration.h>

#include <SoftTwi.h>
#include <RS485Hw.h>

void putc( char c )
{
   Usart::instance<DBG_PORT, DBG_CHANNEL>().write( c );
}

AR8SystemHw::AR8SystemHw()
{
   configure();
}

void AR8SystemHw::configure()
{
   // configure Logger
   Logger::instance().setStream( putc );
   Usart::configPortPins<DBG_PORT, DBG_CHANNEL>();
   Usart::instance<DBG_PORT, DBG_CHANNEL>().init<DBG_BAUDRATE>();

   // configure SPI used by SD card and Ethernet
   Spi::configPortPins<PortC>();

   TRACE_PORT_INIT( AllPins );

   if ( getFckE() <= FCKE_V4_0 )
   {
      // swap red and green LEDs
      greenLed.setPinNumber( 0 );
      redLed.setPinNumber( 1 );
   }
   new DigitalOutputUnit( redLed );


   DEBUG_H1( FSTR( "configure" ) );
   configureSlots();
   configureZeroCrossDetection();
   configureLogicalButtons();
   configureEthernet();
   configureRs485();
   configureTwi();
   IoPort::instance<PortE>().enableInterrupt0();
   enableInterrupts();
}

void AR8SystemHw::configureEthernet()
{
   uint16_t deviceId = HomeAutomationConfiguration::instance().getDeviceId();
   MAC::local.set( 0xAE, 0xDE, 0x48, 0, HBYTE( deviceId ), LBYTE( deviceId ) );

   if ( enc28j60.init() == Enc28j60::OK )
   {
      new HacfIpStackManager( enc28j60 );
      new Gateway( UdpConnection::connect( IP::broadcast(), 9, 9, NULL ), Gateway::UDP_9 );
   }
}

void AR8SystemHw::configureTwi()
{
   if ( getFckE() < FCKE_V4_0 )
   {
      static SoftTwi twi;
      new Gateway( &twi, Gateway::TWI );
   }
}

void AR8SystemHw::configureRs485()
{
   if ( getFckE() >= FCKE_V4_0 )
   {
      DEBUG_M1( FSTR( "RS485" ) );
      Usart::configPortPins<PortE, 0>();
      new Gateway( &rs485Hw, Gateway::RS485 );
   }
}

void AR8SystemHw::configureLogicalButtons()
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

void AR8SystemHw::configureSlots()
{
   DEBUG_M1( FSTR( "Slots" ) );

   uint8_t slot = MAX_SLOTS;
   uint8_t pinNumber = 0;
   Object* object;
   while ( slot-- )
   {
      if ( slot < ( MAX_SLOTS / 2 ) )
      {
         slotHw[slot].getDigitalOutput1()->setPortNumber( PortA );
         slotHw[slot].getDigitalOutput0()->setPortNumber( PortC );
         slotHw[slot].setTimerCounter0( &TimerCounter::instance( PortC, 0 ) );
      }
      else
      {
         slotHw[slot].getDigitalOutput1()->setPortNumber( PortB );
         slotHw[slot].getDigitalOutput0()->setPortNumber( PortD );
         slotHw[slot].setTimerCounter0( &TimerCounter::instance( PortD, 0 ) );
      }
      if ( pinNumber > 3 )
      {
         pinNumber = 0;
      }
      slotHw[slot].getDigitalOutput0()->setPinNumber( pinNumber );
      slotHw[slot].getDigitalOutput1()->setPinNumber( pinNumber );
      slotHw[slot].configure(
         HomeAutomationConfiguration::instance().getSlotType( slot ) );

      if ( slotHw[slot].isDimmerHw() )
      {
         new Dimmer( slot + 1, &slotHw[slot] );
      }
      else if ( slotHw[slot].isPowerSocketHw() )
      {
         object = new DigitalOutputUnit( *slotHw[slot].getDigitalOutput1() );
         object->setInstanceId( slot + 1 );
      }
      else if ( slotHw[slot].isRollerShutterHw() )
      {
         new RollerShutter( slot + 1,
                            reinterpret_cast<RollerShutterHw*>( &slotHw[slot] ) );
      }
      else if ( slotHw[slot].isDoubleSwitchHw() )
      {
         object = new DigitalOutputUnit( *slotHw[slot].getDigitalOutput0() );
         object->setInstanceId( slot + 1 );
         object = new DigitalOutputUnit( *slotHw[slot].getDigitalOutput1() );
         object->setInstanceId( slot + 9 );
      }
      DEBUG_M4( "slot", slot, " = ", slotHw[slot].getType() );

      pinNumber++;
   }
}

void AR8SystemHw::configureZeroCrossDetection()
{
   TimerCounter& phaseShifter = TimerCounter::instance( PortD, 1 );
   phaseShifter.configEventAction( TC_EVSEL_CH1_gc, TC_EVACT_RESTART_gc );
   phaseShifter.configWGM( TC_WGMODE_NORMAL_gc );
   phaseShifter.setPeriod( 0xFFFF );
   // Select TCD1_CCA as event channel 0 multiplexer input.
   EventSystem::setEventSource( 0, EVSYS_CHMUX_TCD1_CCA_gc );

   TimerCounter& dimmerC = TimerCounter::instance( PortC, 0 );
   dimmerC.configWGM( TC_WGMODE_SS_gc );
   dimmerC.configEventAction( TC_EVSEL_CH0_gc, TC_EVACT_RESTART_gc );
   dimmerC.setPeriod( ZCD_DEFAULT_PERIOD );

   TimerCounter& dimmerD = TimerCounter::instance( PortD, 0 );
   dimmerD.configWGM( TC_WGMODE_SS_gc );
   dimmerD.configEventAction( TC_EVSEL_CH0_gc, TC_EVACT_RESTART_gc );
   dimmerD.setPeriod( ZCD_DEFAULT_PERIOD );

   // Select PA7 as event channel 1 multiplexer input.
   DigitalInputTmpl<PortA, 7> pa7;
   IoPort& portA = IoPort::instance( PortA );
   portA.configure( Pin7, PORT_OPC_PULLUP_gc, false, PORT_ISC_RISING_gc );
   portA.enableInterrupt0Source( Pin7 );
   EventSystem::setEventSource( 1, EVSYS_CHMUX_PORTA_PIN7_gc );

   if ( getFckE() < FCKE_V3_0 )
   {
      DigitalInputTmpl<PortA, 5> pa5;
      portA.configure( Pin5, PORT_OPC_PULLUP_gc, false, PORT_ISC_RISING_gc );
      // if slot0-4 has dimmer
      if ( slotHw[0].isDimmerHw() || slotHw[1].isDimmerHw()
         || slotHw[2].isDimmerHw() || slotHw[3].isDimmerHw() )
      {
         // Select PA5 as event channel 0 multiplexer input.
         EventSystem::setEventSource( 1, EVSYS_CHMUX_PORTA_PIN5_gc );
         portA.disableInterrupt0Source( Pin7 );
         portA.enableInterrupt0Source( Pin5 );
      }
      phaseShifter.setCompareA( 0x28 );
   }
   else
   {
      phaseShifter.setCompareA( 0x50 );
   }
   // start counters
   phaseShifter.forceUpdate();
#if F_CPU == 32000000UL
   phaseShifter.configClockSource( TC_CLKSEL_DIV256_gc );
#elif F_CPU == 8000000UL
   phaseShifter.configClockSource( TC_CLKSEL_DIV64_gc );
#else
#    error "F_CPU is not supported for phaseShifter"
#endif

   phaseShifter.setOverflowIntLevel( TC_OVFINTLVL_LO_gc );

   portA.enableInterrupt0( PORT_INT0LVL_MED_gc );
}
