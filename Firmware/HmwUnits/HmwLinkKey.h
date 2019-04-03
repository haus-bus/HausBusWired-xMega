/*
** HBWLinkKey
**
** Einfache direkte Verknuepfung (Peering), vom Tastereingang ausgehend
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/


#ifndef HmwLinkKey_h
#define HmwLinkKey_h

#include "HmwLinkSender.h"

class HmwLinkKey : public HmwLinkSender
{
   public:

      struct Config
      {
         uint8_t ownChannel;
         uint32_t actorAddress;
         uint8_t actorChannel;
      };

      HmwLinkKey( uint8_t _numLinks, Config* _links );
      IStream::Status sendKeyEvent( uint8_t srcChan, uint8_t keyPressNum, bool longPress );



   private:
      uint8_t numLinks;         // number of links of this type
      Config* links;            // size sollte konstant sein -> als define in .cpp
};

#endif
