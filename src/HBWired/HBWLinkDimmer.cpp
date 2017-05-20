/*
** HBWLinkDimmer
**
** Direkte Verknuepfung (Peering), zu Dimmern
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/

#include "HBWLinkDimmer.h"


HBWLinkDimmer::HBWLinkDimmer(uint8_t _numLinks, uint16_t _eepromStart) 
{
	numLinks = _numLinks;
	eepromStart = _eepromStart;
}
 
// processKeyEvent wird aufgerufen, wenn ein Tastendruck empfangen wurde

void HBWLinkDimmer::receiveKeyEvent(HBWDevice* device, uint32_t senderAddress, uint8_t senderChannel, 
                                          uint8_t targetChannel, boolean longPress) 
{

  Config data;

  // read what to do from EEPROM
  for(byte i = 0; i < numLinks; i++) 
  {
	  device->readEEPROM(&data, eepromStart + sizeof(data) * i, sizeof(data) );

	  // compare sender channel
	  if(data.sensorChannel != senderChannel) continue;

	  // compare target channel
	  if(data.ownChannel != targetChannel) continue;

      if(data.sensorAddress == 0xFFFFFFFF) continue;

      // the endianess in the EEPROM is BigEndian, we need it in LittleEndian
      flipEndianess( &data.sensorAddress );
	  if(data.sensorAddress != senderAddress) continue;

	  // ok, we have found a match
	  // differs for short and long
      uint8_t value = 255;
   	  if( longPress )
      {
      	  // we can have
      	  switch( data.longActionType ) 
          {
          	  case 0: // -> ON
              {
                value = data.longOnLevel;
          	    break;
              }
          	  case 1: // -> OFF
              {
                value = data.longOffLevel;
          	    break;
              }
              case 3: // -> TOGGLE
              {
                break;
              }
              case 2: // -> INACTIVE
          	  default: 
              {
                continue;
              }
           }
      }
      else
      {
      	  switch( data.shortActionType )
      	  {
          	  case 0: // -> ON
          	  {
              	  value = data.shortOnLevel;
              	  break;
          	  }
          	  case 1: // -> OFF
          	  {
              	  value = data.shortOffLevel;
              	  break;
          	  }
              case 3: // -> TOGGLE
              {
                  break;
              }
              case 2: // -> INACTIVE
              default:
              {
                  continue;
              }
      	  }
      }
	  device->set(targetChannel,1,&value);    // channel, data length, data
  }
}
 

