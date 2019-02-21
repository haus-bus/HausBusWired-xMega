/*
** HmwLinkDimmer
**
** Direkte Verknuepfung (Peering), zu Dimmern
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/

#ifndef HmwLinkDimmer_h
#define HmwLinkDimmer_h

#include "HmwLinkReceiver.h"
#include "HmwDimmer.h"

class HmwLinkDimmer : public HmwLinkReceiver
{
   public:

      struct Config  // sizeof = 54Bytes
      {
         uint32_t sensorAddress;
         uint8_t sensorChannel;
         uint8_t ownChannel;
         HmwDimmer::ActionParameter shortActionParameter;
         HmwDimmer::ActionParameter longActionParameter;
      };

      HmwLinkDimmer( uint8_t _numLinks, Config* _links );
      void receiveKeyEvent( const uint32_t&  senderAddress, uint8_t senderChannel, uint8_t targetChannel, bool longPress, uint8_t keyNum );

   private:
      uint8_t numLinks;         // number of links of this type
      Config* links;
};

#endif
