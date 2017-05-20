/* 
** HBWLinkKey
**
** Einfache direkte Verknuepfung (Peering), vom Tastereingang ausgehend
** Ein Link-Objekt steht immer fuer alle (direkt aufeinander folgenden) Verknuepfungen
** des entsprechenden Typs.
**
*/


#include "HBWLinkKey.h"


HBWLinkKey::HBWLinkKey(uint8_t _numLinks, uint16_t _eepromStart) {
	numLinks = _numLinks;
	eepromStart = _eepromStart;
}
 

// keyPressed wird aufgerufen, wenn ein Tastendruck erkannt wurde
void HBWLinkKey::sendKeyEvent(HBWDevice* device, uint8_t srcChan, 
                                    uint8_t keyPressNum, boolean longPress) {
	Config data;

    // care for peerings
	for(int i = 0; i < numLinks; i++) 
    {
		// read peering config
		device->readEEPROM( &data, eepromStart + sizeof(data) * i, sizeof(data) );
		// valid peering?
		// TODO: Really like that? This always goes through all possible peerings
		if(data.ownChannel == 0xFF) continue;

		// channel is key?
		if(data.ownChannel != srcChan) continue;

		// the endianess in the EEPROM is BigEndian, we need it in LittleEndian
        flipEndianess( &data.actorAddress );

        // own address? -> internal peering
		if( data.actorAddress == device->getOwnAddress()) 
        {
			device->receiveKeyEvent( data.actorAddress, srcChan, data.actorChannel, longPress );
		}
        else
        {
			// external peering
			// TODO: If bus busy, then try to repeat. ...aber zuerst feststellen, wie das die Original-Module machen (bzw. hier einfach so lassen)
			/* byte result = */ 
			device->sendKeyEvent( srcChan, keyPressNum, longPress, data.actorAddress, data.actorChannel );
		};
	}; 
}
 

