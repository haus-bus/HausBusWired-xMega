/*
** HBWLinkKey
**
** Einfache direkte Verknuepfung (Peering), vom Tastereingang ausgehend
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/


#include "HBWLinkKey.h"


HBWLinkKey::HBWLinkKey( uint8_t _numLinks, Config* _links )
{
   numLinks = _numLinks;
   links = _links;
}


// keyPressed wird aufgerufen, wenn ein Tastendruck erkannt wurde
void HBWLinkKey::sendKeyEvent( HBWDevice* device, uint8_t srcChan,
                               uint8_t keyPressNum, bool longPress )
{
   // care for peerings
   for ( int i = 0; i < numLinks; i++ )
   {
      // valid peering?
      if ( links[i].ownChannel == 0xFF )
      {
         continue;
      }

      // channel is key?
      if ( links[i].ownChannel != srcChan )
      {
         continue;
      }

      uint32_t actorAddress = changeEndianness( links[i].actorAddress );

      // own address? -> internal peering
      if ( actorAddress == device->getOwnAddress() )
      {
         device->receiveKeyEvent( actorAddress, srcChan, links[i].actorChannel, longPress );
      }
      else
      {
         // external peering
         // TODO: If bus busy, then try to repeat. ...aber zuerst feststellen, wie das die Original-Module machen (bzw. hier einfach so lassen)
         /* byte result = */
         device->sendKeyEvent( srcChan, keyPressNum, longPress, actorAddress, links[i].actorChannel );
      }
   }
}


