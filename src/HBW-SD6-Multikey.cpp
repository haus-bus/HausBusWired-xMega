/*
 * HBW-SD6-Multikey.cpp
 *
 * Created: 03.12.2016 02:31:44
 *  Author: Viktor Pankraz
 */


#define HMW_DEVICETYPE 0xB1

#define HARDWARE_VERSION 0x01
#define FIRMWARE_VERSION 0x0106


#include <Time/SystemTime.h>
#include <Elements/SerialStream.h>
#include <Peripherals/InterruptController.h>

#include "HBW-SD6-MultiKey.h"
#include "HBWired/HBWLinkKey.h"
#include "HBWired/HBWLinkDimmer.h"

// this is the EEPROM layout used by one device
struct hbw_config
{
   HBWSD6Multikey::Config device;       // 0x0000 - 0x000F
   HBWKey::Config keycfg[12];           // 0x0010 - 0x0027
   HBWDimmer::Config ledcfg[12];        // 0x0028 - 0x003F
   HBWDS1820::Config ds1820cfg[6];      // 0x0040 - 0x0063
   HBWAnalogIn::Config analogInCfg[2];  // 0x0064 - 0x006F
   HBWLinkKey::Config keyLinks[40];     // 0x0070 - 0x015F
   HBWLinkDimmer::Config ledLinks[40];  // 0x0160 - 0x03DF
};

static hbw_config& config = *reinterpret_cast<hbw_config*>( MAPPED_EEPROM_START );

#ifdef DEBUG
void putc( char c )
{
   Usart::instance<PortC, 1>().write( c );
}
#endif

HBWDevice* createDevice()
{
   SystemTime::init();
   Eeprom::MemoryMapped::enable();

   // Initialize interfaces
   static SerialStream rs485Stream( &Usart::instance<PortE, 0>(), PortPin( PortE, 2 ), PortPin( PortE, 3 ) );
   rs485Stream.init<19200, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc, USART_CHSIZE_8BIT_gc, true, false>();

#ifdef DEBUG
   static SerialStream debugStream( &Usart::instance<PortC, 1>(), PortPin( PortC, 6 ), PortPin( PortC, 7 ) );
   debugStream.init<115200>();
   Logger::instance().setStream( putc );
#endif

   static HBWKey hbwKey1( PortPin( PortA, 0 ), &( config.keycfg[0] ) );
   static HBWKey hbwKey2( PortPin( PortA, 1 ), &( config.keycfg[1] ) );
   static HBWKey hbwKey3( PortPin( PortA, 2 ), &( config.keycfg[2] ) );
   static HBWKey hbwKey4( PortPin( PortA, 3 ), &( config.keycfg[3] ) );
   static HBWKey hbwKey5( PortPin( PortA, 4 ), &( config.keycfg[4] ) );
   static HBWKey hbwKey6( PortPin( PortA, 5 ), &( config.keycfg[5] ) );

   static HBWKey extHbwKey1( PortPin( PortB, 0 ), &( config.keycfg[6] ) );
   static HBWKey extHbwKey2( PortPin( PortB, 1 ), &( config.keycfg[7] ) );
   static HBWKey extHbwKey3( PortPin( PortB, 2 ), &( config.keycfg[8] ) );
   static HBWKey extHbwKey4( PortPin( PortB, 3 ), &( config.keycfg[9] ) );

#ifdef DEBUG
   // in DEBUG mode these pins are used for serial debug output
   static HBWKey extHbwKey5( PortPin( PortDummy, 6 ), &( config.keycfg[10] ) );
   static HBWKey extHbwKey6( PortPin( PortDummy, 7 ), &( config.keycfg[11] ) );
#else
   static HBWKey extHbwKey5( PortPin( PortC, 6 ), &( config.keycfg[10] ) );
   static HBWKey extHbwKey6( PortPin( PortC, 7 ), &( config.keycfg[11] ) );
#endif

   static HBWDimmer hbwLed1( PortPin( PortC, 0 ), &config.ledcfg[0], true );
   static HBWDimmer hbwLed2( PortPin( PortC, 1 ), &config.ledcfg[1], true );
   static HBWDimmer hbwLed3( PortPin( PortC, 2 ), &config.ledcfg[2], true );
   static HBWDimmer hbwLed4( PortPin( PortC, 3 ), &config.ledcfg[3], true );
   static HBWDimmer hbwLed5( PortPin( PortC, 4 ), &config.ledcfg[4], true );
   static HBWDimmer hbwLed6( PortPin( PortC, 5 ), &config.ledcfg[5], true );

   static HBWDimmer extHbwLed1( PortPin( PortD, 0 ), &config.ledcfg[6] );
   static HBWDimmer extHbwLed2( PortPin( PortD, 1 ), &config.ledcfg[7] );
   static HBWDimmer extHbwLed3( PortPin( PortD, 2 ), &config.ledcfg[8] );
   static HBWDimmer extHbwLed4( PortPin( PortD, 3 ), &config.ledcfg[9] );
   static HBWDimmer extHbwLed5( PortPin( PortD, 4 ), &config.ledcfg[10] );
   static HBWDimmer extHbwLed6( PortPin( PortD, 5 ), &config.ledcfg[11] );

   static OneWire ow( PortPin( PortD, 7 ) );
   static HBWDS1820 hbwTmp1( ow, &config.ds1820cfg[0] );
   static HBWDS1820 hbwTmp2( ow, &config.ds1820cfg[1] );
   static HBWDS1820 hbwTmp3( ow, &config.ds1820cfg[2] );
   static HBWDS1820 hbwTmp4( ow, &config.ds1820cfg[3] );
   static HBWDS1820 hbwTmp5( ow, &config.ds1820cfg[4] );
   static HBWDS1820 hbwTmp6( ow, &config.ds1820cfg[5] );

   // static HBWAnalogIn hbwBrightness(&ADC_BRIGHTNESS, ADC_BRIGHTNESS_CHANNEL, &config.analogInCfg[0] );

   static HBWLinkKey linkSender( sizeof( config.keyLinks ) / sizeof( config.keyLinks[0] ), config.keyLinks );
   static HBWLinkDimmer linkReceiver( sizeof( config.ledLinks ) / sizeof( config.ledLinks[0] ), config.ledLinks );

   static HBWSD6Multikey sd6MultiKey( HMW_DEVICETYPE, HARDWARE_VERSION, FIRMWARE_VERSION,
                                      &rs485Stream, PortPin( PortE, 1 ),
                                      &config.device, &linkSender, &linkReceiver );
   sd6MultiKey.setConfigPins( PortPin( PortA, 4 ), PortPin( PortA, 5 ), PortPin( PortR, 1 ) );
   PortPin( PortR, 1 ).setInverted( true );

   // Authorize interrupts
   InterruptController::enableAllInterruptLevel();
   GlobalInterrupt::enable();
   return &sd6MultiKey;
}

void HBWSD6Multikey::checkConfig()
{
   // set global ledFeedback setting to each key object
   for ( uint8_t i = 0; i < 12; i++ )
   {
      ( (HBWKey*)HBWChannel::getChannel( i ) )->setFeedbackChannel( config.device.ledFeedback ? HBWChannel::getChannel( i + 12 ) : NULL );
   }

   // call base implementation
   HBWDevice::checkConfig();
}


#include <Peripherals/WatchDog.h>
#include <Peripherals/Oscillator.h>
#include <Peripherals/Clock.h>

static void
__attribute__( ( section( ".init3" ), naked, used ) )
lowLevelInit( void )
{
        #ifdef EIND
   __asm volatile ( "ldi r24,pm_hh8(__trampolines_start)\n\t"
                    "out %i0,r24" ::"n" ( &EIND ) : "r24", "memory" );
        #endif
        #ifdef DEBUG
   WatchDog::disable();
        #else
   WatchDog::enable( WatchDog::_4S );
        #endif
   InterruptController::selectAppInterruptSection();

   Clock::configPrescalers( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );

   // Enable internal 32 MHz and 32kHz ring oscillator and wait until they are stable.
   Oscillator::enable( OSC_RC32MEN_bm | OSC_RC32KEN_bm );
   Oscillator::waitUntilOscillatorIsReady( OSC_RC32MEN_bm | OSC_RC32KEN_bm );

   // Set the 32 MHz ring oscillator as the main clock source.
   Clock::selectMainClockSource( CLK_SCLKSEL_RC32M_gc );
}

int main( void )
{
   HBWDevice* device = createDevice();
   while ( 1 )
   {
      device->loop();
      WatchDog::reset();
   }
}