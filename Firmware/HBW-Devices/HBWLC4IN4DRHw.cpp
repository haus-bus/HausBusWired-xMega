/*
 * HBWLC4IN4DRHw.cpp
 *
 * Created: 20.01.2018 11:09:24
 * Author: viktor.pankraz
 */


#include "HBWLC4IN4DRHw.h"

// this is the EEPROM layout used by one device
struct hbw_config
{
   HmwDeviceHw::BasicConfig basicConfig; // 0x0000 - 0x0009
   uint8_tx reserved[6];                 // 0x000A - 0x000F
   HmwKey::Config keyCfg[12];            // 0x0010 - 0x0027
   HmwDimmer::Config lcCfg[12];          // 0x0028 - 0x003F
   uint8_tx reserved2[48];               // 0x0040 - 0x006F
   HmwLinkKey::Config keyLinks[40];      // 0x0070 - 0x015F
   HmwLinkDimmer::Config lcLinks[40];    // 0x0160 - 0x03DF
};

static hbw_config& config = *reinterpret_cast<hbw_config*>( MAPPED_EEPROM_START );

// default constructor
HBWLC4IN4DRHw::HBWLC4IN4DRHw() :
   key1( PortPin( PortA, 0 ), &config.keyCfg[0] ),
   key2( PortPin( PortA, 1 ), &config.keyCfg[1] ),
   key3( PortPin( PortA, 2 ), &config.keyCfg[2] ),
   key4( PortPin( PortA, 3 ), &config.keyCfg[3] ),
   lightControl1( PortPin( PortC, 0 ), &config.lcCfg[0], false, DEFAULT_PWM_RANGE ),
   lightControl2( PortPin( PortC, 1 ), &config.lcCfg[1], false, DEFAULT_PWM_RANGE ),
   lightControl3( PortPin( PortC, 2 ), &config.lcCfg[2], false, DEFAULT_PWM_RANGE ),
   lightControl4( PortPin( PortC, 3 ), &config.lcCfg[3], false, DEFAULT_PWM_RANGE ),
   linkSender( sizeof( config.keyLinks ) / sizeof( config.keyLinks[0] ), config.keyLinks ),
   linkReceiver( sizeof( config.lcLinks ) / sizeof( config.lcLinks[0] ), config.lcLinks )
{
   configLed.setInverted( true );
   configbutton.enablePullup();
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