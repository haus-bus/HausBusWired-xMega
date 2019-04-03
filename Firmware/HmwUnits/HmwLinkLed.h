/*
** HmwLinkLed
**
** Direkte Verknuepfung (Peering), zu Dimmern
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/

#ifndef HmwLinkLed_h
#define HmwLinkLed_h

#include "HmwLinkReceiver.h"

class HmwLinkLed : public HmwLinkReceiver
{
   public:

      struct Config
      {
         uint32_t sensorAddress;
         uint8_t sensorChannel;
         uint8_t ownChannel;
         uint8_t shortActionType : 3;
         uint8_t longActionType : 3;
         uint8_t shortOnLevel;
         uint8_t shortOffLevel;
         uint8_t longOnLevel;
         uint8_t longOffLevel;
         uint8_t blinkOnTime;
         uint8_t blinkOffTime;
         uint8_t blinkQuantity;
         uint8_t reserved[2];
      };

      HmwLinkLed( uint8_t _numLinks, Config* _links );
      void receiveKeyEvent( const uint32_t&  senderAddress, uint8_t senderChannel, uint8_t targetChannel, bool longPress, uint8_t keyPressNum );

   private:
      uint8_t numLinks;         // number of links of this type
      Config* links;
};

#endif
