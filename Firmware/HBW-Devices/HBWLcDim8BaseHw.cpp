/*
 * HmwMultiKeySD6BaseHw.cpp
 *
 * Created: 09.01.2018 18:36:42
 * Author: viktor.pankraz
 */


#include <Peripherals/EventSystem.h>
#include <Peripherals/TimerCounter.h>

#include "HBWLcDim8BaseHw.h"

// this is the EEPROM layout used by one device
struct hbw_config
{
   HmwDeviceHw::BasicConfig basicConfig;  // 0x0000 - 0x0009
   uint8_tx reserved[6];                  // 0x000A - 0x000F
   HmwDimmer::Config dimmerCfg[8];        // 0x0010 - 0x001F
   HmwLinkDimmer::Config dimmerLinks[32]; // 0x0020 - 0x06DF
   HmwDS1820::Config ds1820cfg[6];        // 0x06E0 - 0x0703
                                          // 0x0704 - 0x0800
};

static hbw_config& config = *reinterpret_cast<hbw_config*>( MAPPED_EEPROM_START );

// default constructor
HBWLcDim8BaseHw::HBWLcDim8BaseHw() :

   hbwDimmer1( PortPin( PortC, 3 ), PortPin( PortA, 3 ), &config.dimmerCfg[0] ),
   hbwDimmer2( PortPin( PortC, 2 ), PortPin( PortA, 2 ), &config.dimmerCfg[1] ),
   hbwDimmer3( PortPin( PortC, 1 ), PortPin( PortA, 1 ), &config.dimmerCfg[2] ),
   hbwDimmer4( PortPin( PortC, 0 ), PortPin( PortA, 0 ), &config.dimmerCfg[3] ),
   hbwDimmer5( PortPin( PortD, 3 ), PortPin( PortB, 3 ), &config.dimmerCfg[4] ),
   hbwDimmer6( PortPin( PortD, 2 ), PortPin( PortB, 2 ), &config.dimmerCfg[5] ),
   hbwDimmer7( PortPin( PortD, 1 ), PortPin( PortB, 1 ), &config.dimmerCfg[6] ),
   hbwDimmer8( PortPin( PortD, 0 ), PortPin( PortB, 0 ), &config.dimmerCfg[7] ),

   ow( PortPin( PortE, 4 ) ),
   hbwTmp1( ow, &config.ds1820cfg[0] ),
   hbwTmp2( ow, &config.ds1820cfg[1] ),
   hbwTmp3( ow, &config.ds1820cfg[2] ),
   hbwTmp4( ow, &config.ds1820cfg[3] ),
   hbwTmp5( ow, &config.ds1820cfg[4] ),
   hbwTmp6( ow, &config.ds1820cfg[5] ),

   linkReceiver( sizeof( config.dimmerLinks ) / sizeof( config.dimmerLinks[0] ), config.dimmerLinks )

{
#ifdef _DEBUG_
   TRACE_PORT_INIT( AllPins );
#endif
   configLed.setInverted();
   configButton.enablePullup();
   configureZeroCrossDetection();

} // HBWLcDim8BaseHw

void HBWLcDim8BaseHw::configureZeroCrossDetection()
{
   // config event system and connect the event channel with the right timer channel
   TimerCounter& phaseShifter = TimerCounter::instance( PortD, 1 );
   phaseShifter.configEventAction( TC_EVSEL_CH1_gc, TC_EVACT_RESTART_gc );
   phaseShifter.configWGM( TC_WGMODE_NORMAL_gc );
   phaseShifter.setPeriod( 0xFFFF );
   phaseShifter.setCompareA( 0x50 );

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
   IoPort& portA = IoPort::instance( PortA );
   portA.configure( Pin7, PORT_OPC_PULLUP_gc, false, PORT_ISC_RISING_gc );
   portA.enableInterrupt0Source( Pin7 );
   EventSystem::setEventSource( 1, EVSYS_CHMUX_PORTA_PIN7_gc );

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



HmwDeviceHw::BasicConfig* HBWLcDim8BaseHw::getBasicConfig()
{
   return &config.basicConfig;
}

INTERRUPT void PORTA_INT0_vect()
{
   // check for ZeroCrossDetection is OK
   #if F_CPU == 32000000UL
   TimerCounter::instance( PortC, 0 ).configClockSource( TC_CLKSEL_DIV8_gc );
   TimerCounter::instance( PortD, 0 ).configClockSource( TC_CLKSEL_DIV8_gc );
   #elif F_CPU == 8000000UL
   TimerCounter::instance( PortC, 0 ).configClockSource( TC_CLKSEL_DIV2_gc );
   TimerCounter::instance( PortD, 0 ).configClockSource( TC_CLKSEL_DIV2_gc );
   #else
   #    error "F_CPU is not supported for ZeroCrossDetection"
   #endif
}

INTERRUPT void TCD1_OVF_vect()
{
   // if we get here, zero cross detection fails
   TimerCounter::instance( PortC, 0 ).configClockSource( TC_CLKSEL_OFF_gc );
   TimerCounter::instance( PortD, 0 ).configClockSource( TC_CLKSEL_OFF_gc );
}