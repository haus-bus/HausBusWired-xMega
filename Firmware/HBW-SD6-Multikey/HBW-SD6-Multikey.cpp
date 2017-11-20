/*
 * HBW-SD6-Multikey.cpp
 *
 * Created: 03.12.2016 02:31:44
 *  Author: Viktor Pankraz
 */

#include <Release.h>
#include <Time/SystemTime.h>
#include <Peripherals/InterruptController.h>
#include <DigitalOutput.h>

#include <HBWired/HmwKey.h>
#include <HBWired/HmwDimmer.h>
#include <HBWired/HmwDS1820.h>
#include <HBWired/HmwAnalogIn.h>
#include <HBWired/HmwDevice.h>
#include <HBWired/HmwLinkKey.h>
#include <HBWired/HmwLinkDimmer.h>

#include <Security/ModuleId.h>

extern __attribute__( ( section( ".vectors" ) ) ) const ModuleId moduleId;

const ModuleId moduleId =
{
   "$MOD$ HBW-SD6  ",
   0,
   Release::MAJOR,
   Release::MINOR,
   Release::HMW_SD6_ID,
   0
};

// this is the EEPROM layout used by one device
struct hbw_config
{
   HmwDevice::BasicConfig basicConfig;  // 0x0000 - 0x0009
   uint8_tx reserved[6];                // 0x000A - 0x000F
   HmwKey::Config keycfg[12];           // 0x0010 - 0x0027
   HmwDimmer::Config ledcfg[12];        // 0x0028 - 0x003F
   HmwDS1820::Config ds1820cfg[6];      // 0x0040 - 0x0063
   HmwAnalogIn::Config analogInCfg[2];  // 0x0064 - 0x006F
   HmwLinkKey::Config keyLinks[40];     // 0x0070 - 0x015F
   HmwLinkDimmer::Config ledLinks[40];  // 0x0160 - 0x03DF
};

static hbw_config& config = *reinterpret_cast<hbw_config*>( MAPPED_EEPROM_START );

#ifdef DEBUG
void putc( char c )
{
   Usart::instance<PortC, 1>().write( c );
}
#endif

void createDevice()
{
   SystemTime::init();
   Eeprom::MemoryMapped::enable();

   HmwDevice::deviceType = Release::HMW_SD6_ID;
   HmwDevice::firmwareVersion = ( ( Release::MAJOR << 8 ) | Release::MINOR );
   HmwDevice::hardwareVersion = Release::REV_0;
   HmwDevice::basicConfig = reinterpret_cast<HmwDevice::BasicConfig*>( MAPPED_EEPROM_START );
   HmwDevice::ownAddress = changeEndianness( HmwDevice::basicConfig->ownAdress );

   // Initialize interfaces
   DigitalOutput rxEnable( PortE, 0 ), txEnable( PortE, 1 );
   HmwStream::setStream( Usart::instance<PortE, 0>() );

#ifdef DEBUG
   static SerialStream debugStream( &Usart::instance<PortC, 1>(), PortPin( PortC, 6 ), PortPin( PortC, 7 ) );
   debugStream.init<115200>();
   Logger::instance().setStream( putc );
#endif

   static HmwKey hbwKey1( PortPin( PortA, 0 ), &( config.keycfg[0] ) );
   static HmwKey hbwKey2( PortPin( PortA, 1 ), &( config.keycfg[1] ) );
   static HmwKey hbwKey3( PortPin( PortA, 2 ), &( config.keycfg[2] ) );
   static HmwKey hbwKey4( PortPin( PortA, 3 ), &( config.keycfg[3] ) );
   static HmwKey hbwKey5( PortPin( PortA, 4 ), &( config.keycfg[4] ) );
   static HmwKey hbwKey6( PortPin( PortA, 5 ), &( config.keycfg[5] ) );

   static HmwKey extHbwKey1( PortPin( PortB, 0 ), &( config.keycfg[6] ) );
   static HmwKey extHbwKey2( PortPin( PortB, 1 ), &( config.keycfg[7] ) );
   static HmwKey extHbwKey3( PortPin( PortB, 2 ), &( config.keycfg[8] ) );
   static HmwKey extHbwKey4( PortPin( PortB, 3 ), &( config.keycfg[9] ) );

#ifdef DEBUG
   // in DEBUG mode these pins are used for serial debug output
   static HmwKey extHbwKey5( PortPin( PortDummy, 6 ), &( config.keycfg[10] ) );
   static HmwKey extHbwKey6( PortPin( PortDummy, 7 ), &( config.keycfg[11] ) );
#else
   static HmwKey extHbwKey5( PortPin( PortC, 6 ), &( config.keycfg[10] ) );
   static HmwKey extHbwKey6( PortPin( PortC, 7 ), &( config.keycfg[11] ) );
#endif

   static HmwDimmer hbwLed1( PortPin( PortC, 0 ), &config.ledcfg[0], true );
   static HmwDimmer hbwLed2( PortPin( PortC, 1 ), &config.ledcfg[1], true );
   static HmwDimmer hbwLed3( PortPin( PortC, 2 ), &config.ledcfg[2], true );
   static HmwDimmer hbwLed4( PortPin( PortC, 3 ), &config.ledcfg[3], true );
   static HmwDimmer hbwLed5( PortPin( PortC, 4 ), &config.ledcfg[4], true );
   static HmwDimmer hbwLed6( PortPin( PortC, 5 ), &config.ledcfg[5], true );

   static HmwDimmer extHbwLed1( PortPin( PortD, 0 ), &config.ledcfg[6] );
   static HmwDimmer extHbwLed2( PortPin( PortD, 1 ), &config.ledcfg[7] );
   static HmwDimmer extHbwLed3( PortPin( PortD, 2 ), &config.ledcfg[8] );
   static HmwDimmer extHbwLed4( PortPin( PortD, 3 ), &config.ledcfg[9] );
   static HmwDimmer extHbwLed5( PortPin( PortD, 4 ), &config.ledcfg[10] );
   static HmwDimmer extHbwLed6( PortPin( PortD, 5 ), &config.ledcfg[11] );

   static OneWire ow( PortPin( PortD, 7 ) );
   static HmwDS1820 hbwTmp1( ow, &config.ds1820cfg[0] );
   static HmwDS1820 hbwTmp2( ow, &config.ds1820cfg[1] );
   static HmwDS1820 hbwTmp3( ow, &config.ds1820cfg[2] );
   static HmwDS1820 hbwTmp4( ow, &config.ds1820cfg[3] );
   static HmwDS1820 hbwTmp5( ow, &config.ds1820cfg[4] );
   static HmwDS1820 hbwTmp6( ow, &config.ds1820cfg[5] );

   static HmwLinkKey linkSender( sizeof( config.keyLinks ) / sizeof( config.keyLinks[0] ), config.keyLinks );
   static HmwLinkDimmer linkReceiver( sizeof( config.ledLinks ) / sizeof( config.ledLinks[0] ), config.ledLinks );

   // sd6MultiKey.setConfigPins( PortPin( PortA, 4 ), PortPin( PortA, 5 ), PortPin( PortR, 1 ) );
   PortPin( PortR, 1 ).setInverted( true );

   // set ledFeedback channels
   for ( uint8_t i = 0; i < 12; i++ )
   {
      ( (HmwKey*)HmwChannel::getChannel( i ) )->setFeedbackChannel( HmwChannel::getChannel( i + 12 ) );
   }

   // Authorize interrupts
   InterruptController::selectAppInterruptSection();
   InterruptController::enableAllInterruptLevel();
   GlobalInterrupt::enable();
}


int main( void )
{
   createDevice();
   while ( 1 )
   {
      HmwDevice::loop();
   }
}