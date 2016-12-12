/*
 * HMWRS485.h
 *
 *  Created on: 09.05.2014
 *      Author: thorsten
 */

#ifndef HMWRS485_H_
#define HMWRS485_H_

#include <stdint.h>
#include <serial.h>

#define MAX_RX_FRAME_LENGTH 64

// TODO: Do the device relations really belong here?
// TODO: Wo werden die Device Relations gesetzt? Irgendwo im EEPROM?
// #define MAX_DEVICE_RELATIONS 100  TODO: loeschen, wenn nicht gebraucht

#include "HMWModuleInterface.h"

class HMWRS485
{
public:
  HMWRS485( usart_if, uint8_t, uint8_t );  // RS485 interface, RX-Enable Pin, TX-Enable Pin

  void loop(); // main loop, die immer wieder aufgerufen werden muss

  // sendFrame macht ggf. Wiederholungen
  void sendFrame();
  void sendAck();  // ACK fuer gerade verarbeitete Message senden

  // Modul-Definition, wird vom Modul selbst gesetzt
  // TODO: Ist das gutes Design?
  HMWModuleInterface* module;

  // Senderadresse beim Empfangen
  // TODO: Das sollte private sein, wird aber momentan noch vom Modul verwendet
  uint32_t senderAddress;

  // Senden
  uint32_t txSenderAddress;			// eigene Adresse
  uint32_t txTargetAddress;			// Adresse des Moduls
  uint8_t txFrameControlByte;
  uint8_t txFrameDataLength;        // Laenge der Daten + Checksum
  uint8_t txFrameData[MAX_RX_FRAME_LENGTH];

private:
// Das eigentliche RS485-Interface, kann SoftSerial oder (Hardware)Serial sein
  usart_if serial;
// Pin-Nummer fuer "RX/TX-Enable"
  uint8_t rxEnablePin;
  uint8_t txEnablePin;
  // Empfangs-Status
  uint8_t frameStatus;
// Empfangene Daten
  // Empfangen
  uint8_t frameComplete;
  uint32_t targetAddress;
  uint8_t frameDataLength;                 // Laenge der Daten
  uint8_t frameData[MAX_RX_FRAME_LENGTH];
  uint8_t startByte;
  uint8_t frameControlByte;

  // Sende-Versuch, wird ggf. wiederholt
  void receive();  // wird zyklisch aufgerufen
  bool parseFrame();

  void sendFrameSingle();
  void sendFrameByte( uint8_t );
  unsigned int crc16Shift( uint8_t, uint16_t );
};

#endif /* HMWRS485_H_ */
