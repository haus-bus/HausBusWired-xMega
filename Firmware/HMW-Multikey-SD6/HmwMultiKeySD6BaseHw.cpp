/*
 * HmwMultiKeySD6BaseHw.cpp
 *
 * Created: 09.01.2018 18:36:42
 * Author: viktor.pankraz
 */


#include "HmwMultiKeySD6BaseHw.h"

// this is the EEPROM layout used by one device
struct hbw_config
{
   HmwDeviceHw::BasicConfig basicConfig; // 0x0000 - 0x0009
   uint8_tx reserved[6];                 // 0x000A - 0x000F
   HmwKey::Config keycfg[12];            // 0x0010 - 0x0027
   HmwDimmer::Config ledcfg[12];         // 0x0028 - 0x003F
   HmwDS1820::Config ds1820cfg[6];       // 0x0040 - 0x0063
   HmwAnalogIn::Config analogInCfg[2];   // 0x0064 - 0x006F
   HmwLinkKey::Config keyLinks[40];      // 0x0070 - 0x015F
   HmwLinkDimmer::Config ledLinks[40];   // 0x0160 - 0x03DF
};

static hbw_config& config = *reinterpret_cast<hbw_config*>( MAPPED_EEPROM_START );

void debug( char c )
{
   Usart::instance<PortD, 1>().write( c );
}

uint8_t HmwMultiKeySD6BaseHw::getVersion()
{
   // allow also 0xFF that is REV_0 but with an old booter, that did not set correct revision
   if ( config.basicConfig.hwVersion == 0xFF )
   {
      // update the cell so that HW_REV is reported correctly
      config.basicConfig.hwVersion = Release::REV_0;
   }
   return config.basicConfig.hwVersion;
}

// default constructor
HmwMultiKeySD6BaseHw::HmwMultiKeySD6BaseHw( PortPin txEnablePin, PortPin owPin ) :
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

   hbwLed1( PortPin( PortC, 0 ), &config.ledcfg[0], true ),
   hbwLed2( PortPin( PortC, 1 ), &config.ledcfg[1], true ),
   hbwLed3( PortPin( PortC, 2 ), &config.ledcfg[2], true ),
   hbwLed4( PortPin( PortC, 3 ), &config.ledcfg[3], true ),
   hbwLed5( PortPin( PortC, 4 ), &config.ledcfg[4], true ),
   hbwLed6( PortPin( PortC, 5 ), &config.ledcfg[5], true ),

   extHbwLed1( PortPin( PortD, 0 ), &config.ledcfg[6] ),
   extHbwLed2( PortPin( PortD, 1 ), &config.ledcfg[7] ),
   extHbwLed3( PortPin( PortD, 2 ), &config.ledcfg[8] ),
   extHbwLed4( PortPin( PortD, 3 ), &config.ledcfg[9] ),
   extHbwLed5( PortPin( PortD, 4 ), &config.ledcfg[10] ),
   extHbwLed6( PortPin( PortD, 5 ), &config.ledcfg[11] ),

   ow( owPin ),
   hbwTmp1( ow, &config.ds1820cfg[0] ),
   hbwTmp2( ow, &config.ds1820cfg[1] ),
   hbwTmp3( ow, &config.ds1820cfg[2] ),
   hbwTmp4( ow, &config.ds1820cfg[3] ),
   hbwTmp5( ow, &config.ds1820cfg[4] ),
   hbwTmp6( ow, &config.ds1820cfg[5] ),

   linkSender( sizeof( config.keyLinks ) / sizeof( config.keyLinks[0] ), config.keyLinks ),
   linkReceiver( sizeof( config.ledLinks ) / sizeof( config.ledLinks[0] ), config.ledLinks ),

   txEnable( txEnablePin )
{
   // setup debug console
#ifdef DEBUG
   DigitalInputTmpl< PortD, 6 > rxDebug;
   DigitalOutputTmpl<PortD, 7> txDebug;
   Usart::instance<PortD, 1>().init<115200>();
   Logger::instance().setStream( debug );
#endif

   // setup the serial for HmwStream, the txEnable/rxEnable lines have to be set in the special HW version
   DigitalInputTmpl< PortE, 2 > rxHmwStream;
   DigitalOutputTmpl<PortE, 3> txHmwStream;
   serial = &Usart::instance<PortE, 0>();

} // HmwMultiKeySD6BaseHw


HmwDeviceHw::BasicConfig* HmwMultiKeySD6BaseHw::getBasicConfig()
{
   return &config.basicConfig;
}

void HmwMultiKeySD6BaseHw::enableTranceiver( bool enable )
{
   enable ? txEnable.set() : txEnable.clear();
}

SIGNAL(USARTE0_RXC_vect)
{
   HmwStream::nextByteReceivedFromISR(Usart::instance<PortE,0>().readDataRegisterFromISR());
}