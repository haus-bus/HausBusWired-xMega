/*
 * HBWLC4IN4DRHw.cpp
 *
 * Created: 20.01.2018 11:09:24
 * Author: viktor.pankraz
 */


#include "HBWLC4IN4DRHw.h"
#include <Peripherals/TimerCounter.h>

// this is the EEPROM layout used by one device
struct hbw_config
{
   HmwDeviceHw::BasicConfig basicConfig; // 0x0000 - 0x0009
   uint8_tx reserved[6];                 // 0x000A - 0x000F
   HmwKey::Config keyCfg[4];             // 0x0010 - 0x0017
   HmwDimmer::Config ledCfg[4];          // 0x0018 - 0x001F
   HmwLinkKey::Config keyLinks[30];      // 0x0020 - 0x00D3
   HmwLinkDimmer::Config ledLinks[15];   // 0x00D4 - 0x03FD
};

static hbw_config& config = *reinterpret_cast<hbw_config*>( MAPPED_EEPROM_START );

static const uint8_t NORMALIZE_LEVEL = 8;

// default constructor
HBWLC4IN4DRHw::HBWLC4IN4DRHw( uint16_t additionalPeriodValue ) :
   key1( PortPin( PortA, 0 ), &config.keyCfg[0] ),
   key2( PortPin( PortA, 1 ), &config.keyCfg[1] ),
   key3( PortPin( PortA, 2 ), &config.keyCfg[2] ),
   key4( PortPin( PortA, 3 ), &config.keyCfg[3] ),
   lightControl1( PortPin( PortC, 0 ), PortPin( PortDummy, 0 ), &config.ledCfg[0], NORMALIZE_LEVEL ),
   lightControl2( PortPin( PortC, 1 ), PortPin( PortDummy, 1 ), &config.ledCfg[1], NORMALIZE_LEVEL ),
   lightControl3( PortPin( PortC, 2 ), PortPin( PortDummy, 2 ), &config.ledCfg[2], NORMALIZE_LEVEL ),
   lightControl4( PortPin( PortC, 3 ), PortPin( PortDummy, 3 ), &config.ledCfg[3], NORMALIZE_LEVEL ),
   linkSender( sizeof( config.keyLinks ) / sizeof( config.keyLinks[0] ), config.keyLinks ),
   linkReceiver( sizeof( config.ledLinks ) / sizeof( config.ledLinks[0] ), config.ledLinks )
{
   configLed.setInverted( true );
   configbutton.enablePullup();

   TimerCounter& dimmerC = TimerCounter::instance( PortC, 0 );
   dimmerC.configWGM( TC_WGMODE_SS_gc );
   dimmerC.setPeriod( (uint16_t)HmwDimmer::MAX_LEVEL* NORMALIZE_LEVEL + additionalPeriodValue );
   dimmerC.configClockSource( TC_CLKSEL_DIV1_gc );
}


HmwDeviceHw::BasicConfig* HBWLC4IN4DRHw::getBasicConfig()
{
   return &config.basicConfig;
}

void HBWLC4IN4DRHw::enableTranceiver( bool enable )
{
   enable ? txEnable.set() : txEnable.clear();
}

void HBWLC4IN4DRHw::notifyConfigButtonState( ConfigButtonState state )
{
   static Timestamp lastLEDtime;

   switch ( state )
   {
      case IDLE:
      {
         configLed.clear();
         break;
      }

      case FIRST_PRESS:
      {
         configLed.set();
         break;
      }

      case FIRST_LONG_PRESS:
      case WAIT_SECOND_PRESS:
      case SECOND_PRESS:
      {
         if ( lastLEDtime.since() > 500 )
         {
            configLed.toggle();
            lastLEDtime = Timestamp();
         }
         break;
      }

      case SECOND_LONG_PRESS:
      {
         if ( lastLEDtime.since() > 200 )
         {
            configLed.toggle();
            lastLEDtime = Timestamp();
         }
         break;
      }
      default:
      {
         // nothing to do
      }
   }
}