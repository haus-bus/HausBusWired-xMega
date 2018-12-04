/*
 * HmwMultiKeySD6BaseHw.cpp
 *
 * Created: 09.01.2018 18:36:42
 * Author: viktor.pankraz
 */


#include <Peripherals/EventSystem.h>

#include "HBWLcDim8BaseHw.h"

// this is the EEPROM layout used by one device
struct hbw_config
{
   HmwDeviceHw::BasicConfig basicConfig;  // 0x0000 - 0x0009
   uint8_tx reserved[6];                  // 0x000A - 0x000F
   HmwDimmer::Config dimmerCfg[8];           // 0x0010 - 0x0027
};

static hbw_config& config = *reinterpret_cast<hbw_config*>( MAPPED_EEPROM_START );

// default constructor
HBWLcDim8BaseHw::HBWLcDim8BaseHw() :

   hbwDimmer1( PortPin( PortC, 0 ), &config.dimmerCfg[0] ),
   hbwDimmer2( PortPin( PortC, 1 ), &config.dimmerCfg[1] ),
   hbwDimmer3( PortPin( PortC, 2 ), &config.dimmerCfg[2] ),
   hbwDimmer4( PortPin( PortC, 3 ), &config.dimmerCfg[3] ),
   hbwDimmer5( PortPin( PortC, 4 ), &config.dimmerCfg[4] ),
   hbwDimmer6( PortPin( PortC, 5 ), &config.dimmerCfg[5] ),
   hbwDimmer7( PortPin( PortC, 5 ), &config.dimmerCfg[6] ),
   hbwDimmer8( PortPin( PortC, 5 ), &config.dimmerCfg[7] )

{
#ifdef DEBUG
   TRACE_PORT_INIT( AllPins );
#endif
   // config event system and connect the event channel with the right timer channel


} // HBWLcDim8BaseHw


HmwDeviceHw::BasicConfig* HBWLcDim8BaseHw::getBasicConfig()
{
   return &config.basicConfig;
}

