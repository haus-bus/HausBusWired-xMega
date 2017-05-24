/* 
** HBWLinkDimmer
**
** Direkte Verknuepfung (Peering), zu Dimmern
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/

#ifndef HBWLinkDimmer_h
#define HBWLinkDimmer_h

#include "HBWired.h"

class HBWLinkDimmer : public HBWLinkReceiver {
  public:
      HBWLinkDimmer(uint8_t _numLinks, uint16_t _eepromStart);
	  void receiveKeyEvent(HBWDevice* device, uint32_t senderAddress, uint8_t senderChannel, 
                           uint8_t targetChannel, boolean longPress);					   
  private:

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

      uint8_t numLinks;         // number of links of this type  
      uint16_t eepromStart;     //size sollte konstant sein -> als define in .cpp
};

#endif
