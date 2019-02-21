/*
** HBWLinkDimmer
**
** Direkte Verknuepfung (Peering), zu Dimmern
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/

#include "HmwLinkDimmer.h"
#include "HmwChannel.h"
#include "HmwDevice.h"


HmwLinkDimmer::HmwLinkDimmer( uint8_t _numLinks, Config* _links )
{
   numLinks = _numLinks;
   links = _links;
}

// processKeyEvent wird aufgerufen, wenn ein Tastendruck empfangen wurde

void HmwLinkDimmer::receiveKeyEvent( const uint32_t&  senderAddress, uint8_t senderChannel, uint8_t targetChannel, bool longPress, uint8_t keyNum )
{
   // read what to do from EEPROM
   for ( uint8_t i = 0; i < numLinks; i++ )
   {
      // compare sender channel
      if ( links[i].sensorChannel != senderChannel )
      {
         continue;
      }

      // compare target channel
      if ( links[i].ownChannel != targetChannel )
      {
         continue;
      }

      if ( links[i].sensorAddress == 0xFFFFFFFF )
      {
         continue;
      }

      // the endianess in the EEPROM is BigEndian, we need it in LittleEndian
      if ( changeEndianness( links[i].sensorAddress ) != senderAddress )
      {
         continue;
      }

      HmwDimmer::LinkCommand cmd;
      cmd.keyNum = keyNum;
      cmd.actionParameter = ( longPress ? &links[i].longActionParameter : &links[i].shortActionParameter );

      HmwDevice::set( targetChannel, sizeof( cmd ), (uint8_t*)&cmd );    // channel, data length, data
   }
}


