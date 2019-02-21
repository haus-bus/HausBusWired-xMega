/*
** HBWLinkLed
**
** Direkte Verknuepfung (Peering), zu Leds
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/

#include "HmwLinkLed.h"
#include "HmwChannel.h"
#include "HmwDevice.h"


HmwLinkLed::HmwLinkLed( uint8_t _numLinks, Config* _links )
{
   numLinks = _numLinks;
   links = _links;
}

// processKeyEvent wird aufgerufen, wenn ein Tastendruck empfangen wurde

void HmwLinkLed::receiveKeyEvent( const uint32_t&  senderAddress, uint8_t senderChannel, uint8_t targetChannel, bool longPress, uint8_t keyPressNum )
{
   static uint8_t lastKeyPressNum = -1;

   // broadcast events are sent repeated for long press
   if ( lastKeyPressNum == keyPressNum )
   {
      return;
   }

   lastKeyPressNum = keyPressNum;

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

      // ok, we have found a match
      // differs for short and long
      uint8_t cmdData[] = { 255, links[i].shortOffLevel, links[i].shortOnLevel, links[i].blinkOnTime, links[i].blinkOffTime, links[i].blinkQuantity };
      uint8_t length = sizeof( cmdData );
      if ( longPress )
      {
         cmdData[1] = links[i].longOffLevel;
         cmdData[2] = links[i].longOnLevel;

         // we can have
         switch ( links[i].longActionType )
         {
            case 0:       // -> ON
            {
               cmdData[0] = links[i].longOnLevel;
               length = 1;
               break;
            }
            case 1:       // -> OFF
            {
               cmdData[0] = links[i].longOffLevel;
               length = 1;
               break;
            }
            case 3:   // -> TOGGLE
            {
               cmdData[0] = HmwChannel::TOGGLE;
               break;
            }
            case 4:   // -> BLINK_ON
            {
               cmdData[0] = HmwChannel::BLINK_ON;
               break;
            }
            case 5:   // -> BLINK_TOGGLE
            {
               cmdData[0] = HmwChannel::BLINK_TOGGLE;
               break;
            }


            case 2:   // -> INACTIVE
            default:
            {
               continue;
            }
         }
      }
      else
      {
         switch ( links[i].shortActionType )
         {
            case 0:       // -> ON
            {
               cmdData[0] = links[i].shortOnLevel;
               length = 1;
               break;
            }
            case 1:       // -> OFF
            {
               cmdData[0] = links[i].shortOffLevel;
               length = 1;
               break;
            }
            case 3:   // -> TOGGLE
            {
               cmdData[0] = HmwChannel::TOGGLE;
               break;
            }
            case 4:   // -> BLINK_ON
            {
               cmdData[0] = HmwChannel::BLINK_ON;
               break;
            }
            case 5:   // -> BLINK_TOGGLE
            {
               cmdData[0] = HmwChannel::BLINK_TOGGLE;
               break;
            }
            case 2:   // -> INACTIVE
            default:
            {
               continue;
            }
         }
      }
      HmwDevice::set( targetChannel, length, cmdData );    // channel, data length, data
   }
}


