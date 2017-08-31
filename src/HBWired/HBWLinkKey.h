/*
** HBWLinkKey
**
** Einfache direkte Verknuepfung (Peering), vom Tastereingang ausgehend
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/


#ifndef HBWLinkKey_h
#define HBWLinkKey_h

#include "HBWired.h"

class HBWLinkKey : public HBWLinkSender
{
   public:

      struct Config
      {
         uint8_t ownChannel;
         uint32_t actorAddress;
         uint8_t actorChannel;
      };

      HBWLinkKey( uint8_t _numLinks, Config* _links );
      void sendKeyEvent( HBWDevice* device, uint8_t srcChan, uint8_t keyPressNum, bool longPress );



   private:
      uint8_t numLinks;         // number of links of this type
      Config* links;     // size sollte konstant sein -> als define in .cpp
};

#endif
