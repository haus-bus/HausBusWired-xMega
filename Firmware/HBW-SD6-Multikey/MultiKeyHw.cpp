/*
 * MultiKeyHw.cpp
 *
 * Created: 23.11.2017 22:34:53
 * Author: viktor.pankraz
 */


#include "MultiKeyHw.h"
#include <Tracing/Logger.h>
#include <Time/Timestamp.h>


// this is the EEPROM layout used by one device
struct hbw_config
{
   HmwDevice::BasicConfig basicConfig;   // 0x0000 - 0x0009
   uint8_tx reserved[6];                 // 0x000A - 0x000F
   HmwKey::Config keycfg[12];            // 0x0010 - 0x0027
   HmwDimmer::Config ledcfg[12];         // 0x0028 - 0x003F
   HmwDS1820::Config ds1820cfg[6];       // 0x0040 - 0x0063
   HmwAnalogIn::Config analogInCfg[2];   // 0x0064 - 0x006F
   HmwLinkKey::Config keyLinks[40];      // 0x0070 - 0x015F
   HmwLinkDimmer::Config ledLinks[40];   // 0x0160 - 0x03DF
};

static hbw_config& config = *reinterpret_cast<hbw_config*>( MAPPED_EEPROM_START );
static Usart* debugSerial = &Usart::instance<PortC, 1>();

HmwDevice::BasicConfig* MultiKeyHw::getBasicConfig()
{
   return &config.basicConfig;
}


MultiKeyHw* MultiKeyHw::create()
{
   // Rev0 Pin definitions
   PortPin ppKey1( PortA, 0 );
   PortPin ppKey2( PortA, 1 );
   PortPin ppKey3( PortA, 2 );
   PortPin ppKey4( PortA, 3 );
   PortPin ppKey5( PortA, 4 );
   PortPin ppKey6( PortA, 5 );
   PortPin ppExtKey1( PortB, 0 );
   PortPin ppExtKey2( PortB, 1 );
   PortPin ppExtKey3( PortB, 2 );
   PortPin ppExtKey4( PortB, 3 );
   PortPin ppExtKey5( PortC, 6 );
   PortPin ppExtKey6( PortC, 7 );

   PortPin ppLed1( PortC, 0 );
   PortPin ppLed2( PortC, 1 );
   PortPin ppLed3( PortC, 2 );
   PortPin ppLed4( PortC, 3 );
   PortPin ppLed5( PortC, 4 );
   PortPin ppLed6( PortC, 5 );
   PortPin ppExtLed1( PortD, 0 );
   PortPin ppExtLed2( PortD, 1 );
   PortPin ppExtLed3( PortD, 2 );
   PortPin ppExtLed4( PortD, 3 );
   PortPin ppExtLed5( PortD, 4 );
   PortPin ppExtLed6( PortD, 5 );

   PortPin ppOneWire( PortD, 7 );

   PortPin ppRxEnable( PortE, 0 );
   PortPin ppTxEnable( PortE, 1 );
   PortPin ppRx( PortE, 2 );
   PortPin ppTx( PortE, 3 );

   switch ( getBasicConfig()->hwVersion )
   {
      case Release::REV_0:
      {
         ppRxEnable.configOutput();
#ifdef DEBUG
         ppExtKey5 = PortPin( PortDummy, 6 );
         ppExtKey6 = PortPin( PortDummy, 7 );
         DigitalInputTmpl< PortC, 6 > rx;
         DigitalOutputTmpl<PortC, 7> tx;
         Usart::instance<PortC, 1>().init<115200>();
#endif
         break;
      }

      case Release::REV_1:
      {
         ppTxEnable = PortPin( PortR, 0 );
         ppOneWire = PortPin( PortR, 1 );

         debugSerial = &Usart::instance<PortD, 1>();
         break;
      }

      default:
         while ( 1 )
         {
         }
   }

   Logger::instance().setStream( MultiKeyHw::debug );
   ppRx.configInput();
   ppTx.configOutput();

   static MultiKeyHw hardware( ppTxEnable, &Usart::instance<PortE, 0>() );

   static HmwKey hbwKey1( ppKey1, &( config.keycfg[0] ) );
   static HmwKey hbwKey2( ppKey2, &( config.keycfg[1] ) );
   static HmwKey hbwKey3( ppKey3, &( config.keycfg[2] ) );
   static HmwKey hbwKey4( ppKey4, &( config.keycfg[3] ) );
   static HmwKey hbwKey5( ppKey5, &( config.keycfg[4] ) );
   static HmwKey hbwKey6( ppKey6, &( config.keycfg[5] ) );

   static HmwKey extHbwKey1( ppExtKey1, &( config.keycfg[6] ) );
   static HmwKey extHbwKey2( ppExtKey2, &( config.keycfg[7] ) );
   static HmwKey extHbwKey3( ppExtKey3, &( config.keycfg[8] ) );
   static HmwKey extHbwKey4( ppExtKey4, &( config.keycfg[9] ) );
   static HmwKey extHbwKey5( ppExtKey5, &( config.keycfg[10] ) );
   static HmwKey extHbwKey6( ppExtKey6, &( config.keycfg[11] ) );

   static HmwDimmer hbwLed1( ppLed1, &config.ledcfg[0], true );
   static HmwDimmer hbwLed2( ppLed2, &config.ledcfg[1], true );
   static HmwDimmer hbwLed3( ppLed3, &config.ledcfg[2], true );
   static HmwDimmer hbwLed4( ppLed4, &config.ledcfg[3], true );
   static HmwDimmer hbwLed5( ppLed5, &config.ledcfg[4], true );
   static HmwDimmer hbwLed6( ppLed6, &config.ledcfg[5], true );

   static HmwDimmer extHbwLed1( ppExtLed1, &config.ledcfg[6] );
   static HmwDimmer extHbwLed2( ppExtLed2, &config.ledcfg[7] );
   static HmwDimmer extHbwLed3( ppExtLed3, &config.ledcfg[8] );
   static HmwDimmer extHbwLed4( ppExtLed4, &config.ledcfg[9] );
   static HmwDimmer extHbwLed5( ppExtLed5, &config.ledcfg[10] );
   static HmwDimmer extHbwLed6( ppExtLed6, &config.ledcfg[11] );

   static OneWire ow( ppOneWire );
   static HmwDS1820 hbwTmp1( ow, &config.ds1820cfg[0] );
   static HmwDS1820 hbwTmp2( ow, &config.ds1820cfg[1] );
   static HmwDS1820 hbwTmp3( ow, &config.ds1820cfg[2] );
   static HmwDS1820 hbwTmp4( ow, &config.ds1820cfg[3] );
   static HmwDS1820 hbwTmp5( ow, &config.ds1820cfg[4] );
   static HmwDS1820 hbwTmp6( ow, &config.ds1820cfg[5] );

   // sd6MultiKey.setConfigPins( PortPin( PortA, 4 ), PortPin( PortA, 5 ), PortPin( PortR, 1 ) );

   // set ledFeedback channels
   for ( uint8_t i = 0; i < 12; i++ )
   {
      ( (HmwKey*)HmwChannel::getChannel( i ) )->setFeedbackChannel( HmwChannel::getChannel( i + 12 ) );
   }

   return &hardware;
}

HmwLinkSender* MultiKeyHw::getLinkSender()
{
   static HmwLinkKey linkSender( sizeof( config.keyLinks ) / sizeof( config.keyLinks[0] ), config.keyLinks );
   return &linkSender;
}

HmwLinkReceiver* MultiKeyHw::getLinkReceiver()
{
   static HmwLinkDimmer linkReceiver( sizeof( config.ledLinks ) / sizeof( config.ledLinks[0] ), config.ledLinks );
   return &linkReceiver;
}

void MultiKeyHw::debug( char c )
{
   debugSerial->write( c );
}

void MultiKeyHw::enableTranceiver( bool enable )
{
   enable ? txEnable.set() : txEnable.clear();
}