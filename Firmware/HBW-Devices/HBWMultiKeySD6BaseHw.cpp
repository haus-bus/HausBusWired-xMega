/*
 * HmwMultiKeySD6BaseHw.cpp
 *
 * Created: 09.01.2018 18:36:42
 * Author: viktor.pankraz
 */


#include <Peripherals/EventSystem.h>

#include "HBWMultiKeySD6BaseHw.h"

// this is the EEPROM layout used by one device
struct hbw_config
{
   HmwDeviceHw::BasicConfig basicConfig;  // 0x0000 - 0x0009
   uint8_tx reserved[6];                  // 0x000A - 0x000F
   HmwKey::Config keycfg[12];             // 0x0010 - 0x0027
   HmwLed::Config ledcfg[12];             // 0x0028 - 0x003F
   HmwDS1820::Config ds1820cfg[6];        // 0x0040 - 0x0063
   HmwBrightness::Config brightnessCfg;   // 0x0064 - 0x0069
   HmwSHTC3::Config shtc3Config;          // 0x006A - 0x006F
   HmwLinkKey::Config keyLinks[40];       // 0x0070 - 0x015F
   HmwLinkLed::Config ledLinks[40];       // 0x0160 - 0x03DF
};

static hbw_config& config = *reinterpret_cast<hbw_config*>( MAPPED_EEPROM_START );

// default constructor
HBWMultiKeySD6BaseHw::HBWMultiKeySD6BaseHw( PortPin txEnablePin, PortPin owPin, bool invertLed1To6 ) :
   hbwKey1( PortPin( PortA, 0 ), &( config.keycfg[0] ) ),
   hbwKey2( PortPin( PortA, 1 ), &( config.keycfg[1] ) ),
   hbwKey3( PortPin( PortA, 2 ), &( config.keycfg[2] ) ),
   hbwKey4( PortPin( PortA, 3 ), &( config.keycfg[3] ) ),
   hbwKey5( PortPin( PortA, 4 ), &( config.keycfg[4] ) ),
   hbwKey6( PortPin( PortA, 5 ), &( config.keycfg[5] ) ),

   extHbwKey1( PortPin( PortB, 0 ), &( config.keycfg[6] ) ),
   extHbwKey2( PortPin( PortB, 1 ), &( config.keycfg[7] ) ),
   extHbwKey3( PortPin( PortB, 2 ), &( config.keycfg[8] ) ),
   extHbwKey4( PortPin( PortB, 3 ), &( config.keycfg[9] ) ),
   extHbwKey5( PortPin( PortC, 6 ), &( config.keycfg[10] ) ),
   extHbwKey6( PortPin( PortC, 7 ), &( config.keycfg[11] ) ),

   hbwLed1( PortPin( PortC, 0 ), &config.ledcfg[0], invertLed1To6 ),
   hbwLed2( PortPin( PortC, 1 ), &config.ledcfg[1], invertLed1To6 ),
   hbwLed3( PortPin( PortC, 2 ), &config.ledcfg[2], invertLed1To6 ),
   hbwLed4( PortPin( PortC, 3 ), &config.ledcfg[3], invertLed1To6 ),
   hbwLed5( PortPin( PortC, 4 ), &config.ledcfg[4], invertLed1To6 ),
   hbwLed6( PortPin( PortC, 5 ), &config.ledcfg[5], invertLed1To6 ),

#ifdef DEBUG  // use the IOs PORTD0-PORTD7 for debugging
   extHbwLed1( PortPin( PortDummy, 0 ), &config.ledcfg[6] ),
   extHbwLed2( PortPin( PortDummy, 1 ), &config.ledcfg[7] ),
   extHbwLed3( PortPin( PortDummy, 2 ), &config.ledcfg[8] ),
   extHbwLed4( PortPin( PortDummy, 3 ), &config.ledcfg[9] ),
   extHbwLed5( PortPin( PortDummy, 4 ), &config.ledcfg[10] ),
   extHbwLed6( PortPin( PortDummy, 5 ), &config.ledcfg[11] ),
#else
   extHbwLed1( PortPin( PortD, 0 ), &config.ledcfg[6] ),
   extHbwLed2( PortPin( PortD, 1 ), &config.ledcfg[7] ),
   extHbwLed3( PortPin( PortD, 2 ), &config.ledcfg[8] ),
   extHbwLed4( PortPin( PortD, 3 ), &config.ledcfg[9] ),
   extHbwLed5( PortPin( PortD, 4 ), &config.ledcfg[10] ),
   extHbwLed6( PortPin( PortD, 5 ), &config.ledcfg[11] ),
#endif

   ow( owPin ),
   hbwTmp1( ow, &config.ds1820cfg[0] ),
   hbwTmp2( ow, &config.ds1820cfg[1] ),
   hbwTmp3( ow, &config.ds1820cfg[2] ),
   hbwTmp4( ow, &config.ds1820cfg[3] ),
   hbwTmp5( ow, &config.ds1820cfg[4] ),
   hbwTmp6( ow, &config.ds1820cfg[5] ),

   hbwOnboardBrightness( PortPin( PortA, 6 ), TimerCounterChannel( &TimerCounter::instance( PortE, 0 ), TimerCounter::A ), &config.brightnessCfg ),
   shtc3( Twi::instance<PortE>(), &config.shtc3Config ),

   linkSender( sizeof( config.keyLinks ) / sizeof( config.keyLinks[0] ), config.keyLinks ),
   linkReceiver( sizeof( config.ledLinks ) / sizeof( config.ledLinks[0] ), config.ledLinks ),

   txEnable( txEnablePin )
{
   // config event system and connect the event channel with the right timer channel
   EventSystem::setEventSource( 0, EVSYS_CHMUX_PORTA_PIN6_gc );
   EventSystem::setEventChannelFilter( 0, EVSYS_DIGFILT_8SAMPLES_gc );
   EventSystem::setEventSource( 1, EVSYS_CHMUX_PORTA_PIN7_gc );
   EventSystem::setEventChannelFilter( 1, EVSYS_DIGFILT_8SAMPLES_gc );
   TimerCounter& t = TimerCounter::instance( PortE, 0 );
   t.configWGM( TC_WGMODE_NORMAL_gc );
   t.setPeriod( 0xFFFF );
   t.configClockSource( TC_CLKSEL_DIV1024_gc );
   t.configInputCapture( TC_EVSEL_CH0_gc );

   // setup TWI needed by SHTC3 sensor

   // enable pullup for TWI
   PORTCFG.MPCMASK = Pin0 | Pin1;
   PORTE.PIN0CTRL = PORT_OPC_PULLUP_gc;
   Twi::instance<PortE>().init<true, 100000, TWI_MASTER_INTLVL_OFF_gc, TWI_SLAVE_INTLVL_OFF_gc>();

} // HmwMultiKeySD6BaseHw


HmwDeviceHw::BasicConfig* HBWMultiKeySD6BaseHw::getBasicConfig()
{
   return &config.basicConfig;
}

void HBWMultiKeySD6BaseHw::enableTranceiver( bool enable )
{
   enable ? txEnable.set() : txEnable.clear();
}

