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

class HmwLinkDimmer : public HmwLinkReceiver
{
   public:

      struct Config
      {
         uint32_t sensorAddress;
         uint8_t sensorChannel;
         uint8_t ownChannel;
         uint8_t shortOnTimeMode    : 1;
         uint8_t shortOffTimeMode   : 1;
         uint8_t shortOffDelayMode  : 1;
         uint8_t reserved0          : 1;
         uint8_t shortActionType    : 4;
         uint8_t shortOffLevel;
         uint8_t shortOnMinLevel;
         uint8_t shortOnLevel;
         uint8_t shortRampStartStep;
         uint8_t shortOffDelayStep;
         uint16_t shortOnDelayTime;
         uint16_t shortRampOnTime;
         uint16_t shortOnTime;
         uint16_t shortOffDelayTime;
         uint16_t shortRampOffTime;
         uint16_t shortOffTime;
         uint8_t shortDimMinLevel;
         uint8_t shortDimMaxLevel;
         uint8_t shortDimStep;
         uint8_t shortJtRampOn   : 4;
         uint8_t shortJtOnDelay  : 4;
         uint8_t shortJtOffDelay : 4;
         uint8_t shortJtOn       : 4;
         uint8_t shortJtOff      : 4;
         uint8_t shortJtRampOff  : 4;

         uint8_t longOnTimeMode    : 1;
         uint8_t longOffTimeMode   : 1;
         uint8_t longOnDelayMode   : 1;
         uint8_t longMultiExecute  : 1;
         uint8_t longActionType    : 4;
         uint8_t longOffLevel;
         uint8_t longOnMinLevel;
         uint8_t longOnLevel;
         uint8_t longRampStartStep;
         uint8_t longOffDelayStep;
         uint16_t longOnDelayTime;
         uint16_t longRampOnTime;
         uint16_t longOnTime;
         uint16_t longOffDelayTime;
         uint16_t longRampOffTime;
         uint16_t longOffTime;
         uint8_t longDimMinLevel;
         uint8_t longDimMaxLevel;
         uint8_t longDimStep;
         uint8_t longJtRampOn   : 4;
         uint8_t longJtOnDelay  : 4;
         uint8_t longJtOffDelay : 4;
         uint8_t longJtOn       : 4;
         uint8_t longJtOff      : 4;
         uint8_t longJtRampOff  : 4;
      };

      HmwLinkDimmer( uint8_t _numLinks, Config* _links );
      void receiveKeyEvent( const uint32_t&  senderAddress, uint8_t senderChannel,
                            uint8_t targetChannel, bool longPress );


   private:
      uint8_t numLinks;         // number of links of this type
      Config* links;
};

#endif
