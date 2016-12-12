/* 
* HMWModuleBase.h
*
* Created: 11.12.2016 14:33:02
* Author: haus-bus
*/


#ifndef __HMWMODULEBASE_H__
#define __HMWMODULEBASE_H__

#include <stdint.h>


// Abstrakte Basisklasse mit Callbacks fuer Verwender
class HMWModuleInterface
{
public:
	// processEvent wird nur aufgerufen, wenn es fuer das Modul was zu tun gibt
	// Also nicht fuer ACKs oder duplicates
	// TODO: Should return whether an ACK is needed or not
	virtual void processEvent( uint8_t const * const frameData, uint8_t frameDataLength,
	bool isBroadcast = false ) = 0; // Data, broadcast-Flag
	
}; //HMWModuleBase

#endif //__HMWMODULEBASE_H__
