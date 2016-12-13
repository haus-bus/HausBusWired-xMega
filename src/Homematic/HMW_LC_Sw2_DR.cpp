//*******************************************************************
//
// HMW_LC_Sw1_DR.cpp
//
// Homematic Wired Hombrew Hardware
// Arduino Uno als Homematic-Device
// Thorsten Pferdekaemper (thorsten@pferdekaemper.com)
// nach einer Vorlage von
// Dirk Hoffmann (hoffmann@vmd-jena.de)
//
//-------------------------------------------------------------------
//Hardwarebeschreibung:
// =====================
//
// Pinsettings for Arduino Uno
//
// D0: RXD, normaler Serieller Port fuer Debug-Zwecke
// D1: TXD, normaler Serieller Port fuer Debug-Zwecke
// D5: RXD, RO des RS485-Treiber
// D6: TXD, DI des RS485-Treiber
// D2: Direction (DE/-RE) Driver/Receiver Enable vom RS485-Treiber
//
// A0: Taster 1
// A1: Taster 2
// A2: Schaltausgang 1
// A3: Schaltausgang 2
// D13: oder D13 bei DEBUG_UNO Status-LED

// Die Firmware funktioniert mit dem Standard-Uno Bootloader, im
// Gegensatz zur Homematic-Firmware

//*******************************************************************

#include "Framework/Framework.h"
#include <board.h>

#include "HMW_LC_Sw2_DR.h"

// HM Wired Protokoll
#include "HMWRS485.h"

// default module methods
#include "HMWModule.h"
#include "HMWRegister.h"

static const uint8_t debugLevel( DEBUG_LEVEL_LOW );
#define getId() FSTR("HMW_LcSw2DR::")

#define BUTTON BUTTON_S6_GPIO
#define LED STATUS_LED_GREEN_GPIO
#define IDENTIFY_LED STATUS_LED_RED_GPIO     // Identify LED --> zusätzliche LED für Identifizierung in Verteilung
#define IDENTIFY_EEPROM 0xFF				 // Adresse EEPROM Identify 

#define CHANNEL_IO_COUNT 4
#define CHANNEL_IO_BYTES 1  //CHANNEL_IO_COUNT / 8

// Das folgende Define kann benutzt werden, wenn ueber die
// Kanaele "geloopt" werden soll
// als Define, damit es zentral definiert werden kann, aber keinen (globalen) Speicherplatz braucht
#define CHANNEL_PORTS uint8_t channelPorts[CHANNEL_IO_COUNT] = {BUTTON_S1_GPIO, BUTTON_S2_GPIO, LED1_GPIO, LED2_GPIO};

// Port Status, d.h. Port ist auf 0 oder 1
uint8_t portStatus[CHANNEL_IO_BYTES];

// TODO: wird wirklich int gebraucht oder tut's auch uint8_t?
unsigned int keyLongPressTime[2];
uint8_t loggingTime;

void printChannelConf()
{
	DEBUG_H1( FSTR("printChannel conf"));
	for (uint8_t channel = 0; channel < CHANNEL_IO_COUNT; channel++)
	{
		DEBUG_M2( FSTR("Channel     : "), channel);
	}
}

HMWModule* hmwmodule;   // wird in setup initialisiert

// Config
hmw_config config;

// write config to EEPROM in a hopefully smart way
void writeConfig()
{
//    uint8_t* ptr;
//    uint8_t data;
	// EEPROM lesen und schreiben
//	ptr = (uint8_t*)(sensors);
//	for(int address = 0; address < sizeof(sensors[0]) * CHANNEL_IO_COUNT; address++){
//	  hmwmodule->writeEEPROM(address + 0x10, *ptr);
//	  ptr++;
//  };
}
;

// Read all inputs/outputs
// setzt Bits in portStatus[]
void readPins()
{
	CHANNEL_PORTS
	for (uint8_t i = 0; i < CHANNEL_IO_COUNT; i++)
	{
		// Pin lesen und Bit in portStatus setzen
		// TODO: Check if this really works
		if( ioport_get_value(channelPorts[i]) )
		{
			portStatus[i / 8] |= (1<<(i % 8));
		}
		else
		{
			portStatus[i / 8] &= ~(1<<(i % 8));
		}
	}
}
// Klasse fuer Callbacks vom Protokoll
class HMWDevice: public HMWDeviceInterface
{
public:
	void setLevel(uint8_t channel, unsigned int level)
	{
		// everything in the right limits?
		if (channel != 2 && channel != 3)
			return;
		if (level > 255)
			return;
		// now set pin
		CHANNEL_PORTS
		if (level == 0xFF)
		{   // toggle
			ioport_set_value(channelPorts[channel],
					!ioport_get_value(channelPorts[channel]));
		}
		else if (level) // on
			ioport_set_value(channelPorts[channel], HIGH);
		else
			ioport_set_value(channelPorts[channel], LOW);
	}

	unsigned int getLevel(uint8_t channel)
	{
		// everything in the right limits?
		if (channel >= CHANNEL_IO_COUNT)
			return 0;
		// read
		CHANNEL_PORTS
		if (ioport_get_value(channelPorts[channel]))
			return 0xC800;
		else
			return 0;
	}
	;

	void readConfig()
	{
		uint8_t* ptr;
		// EEPROM lesen
		ptr = (uint8_t*) (&config);
		for (unsigned long address = 0; address < sizeof(config); address++)
		{
			nvm_read_char(INT_EEPROM, address + 0x01, ptr++ );
		};
		// defaults setzen, falls nicht sowieso klar
		if (config.logging_time == 0xFF)
			config.logging_time = 20;
		if (config.central_address == 0xFFFFFFFF)
			config.central_address = 0x00000001;
		for (uint8_t channel = 0; channel < HMW_CONFIG_NUM_KEYS; channel++)
		{
			if (config.keys[channel].long_press_time == 0xFF)
				config.keys[channel].long_press_time = 10;
		};
	}
	;

};

//The setup function is called once at startup of the sketch
void setup( HMWRS485* hmwrs485 )
{
	DEBUG_H1(FSTR("setup()"));
	//-------------------------------------------------------------
	// device type: 0x11 = HMW-LC-Sw2-DR
	static HMWDevice hmwdevice;
	static HMWModule modul(&hmwdevice, hmwrs485, 0x11);
	// config aus EEPROM lesen
	hmwdevice.readConfig();
	hmwmodule = &modul;
// "HHB2703111", 0x42380123);

	hmwmodule->broadcastAnnounce(0);
	
	DEBUG_M1(FSTR("started"));
}

//------------------------------------------------------------
// Hinzugefügt für initialisierung

void factoryReset()
{
	// writes FF into config
//  memset(sensors, 0xFF, sizeof(sensors[0]) * CHANNEL_IO_COUNT);
	// set defaults
//  setDefaults();
}

void handleButton()
{
	// langer Tastendruck (5s) -> LED blinkt hektisch
	// dann innerhalb 10s langer Tastendruck (3s) -> LED geht aus, EEPROM-Reset

//-----------------------------------------------------------------------
	// Erweiterung für Identity LED
	// Identity Status und EEPROM Adresse 255 lesen
	static uint8_t IDENTY;
	static long last_IDENT_LED_time;
	SystemTime::time_t now = SystemTime::now();

    nvm_read_char(INT_EEPROM, IDENTIFY_EEPROM, &IDENTY ); //EEPROM Adresse 255 lesen
	
	if (IDENTY == 0x00)
	{
		ioport_set_value(IDENTIFY_LED, LOW);
		last_IDENT_LED_time = now;
	}
	else
	{
		if (now - last_IDENT_LED_time > 600)
		{
			ioport_set_value(IDENTIFY_LED, !ioport_get_value(IDENTIFY_LED)); //Toggel LED
			last_IDENT_LED_time = now;                  // Zähler zurücksetzen
		}
	}

//-----------------------------------------------------------------------

	static long lastTime = 0;
	static uint8_t status = 0; // 0: normal, 1: Taste erstes mal gedrückt, 2: erster langer Druck erkannt
							// 3: Warte auf zweiten Tastendruck, 4: Taste zweites Mal gedrückt
							// 5: zweiter langer Druck erkannt

	bool buttonState = !ioport_get_value(BUTTON);

	switch (status)
	{
	case 0:
		if (buttonState)
		{
			status = 1;
			DEBUG_M2( "ButtonState ", status);
		}
		lastTime = now;
		break;
	case 1:
		if (buttonState)
		{   // immer noch gedrueckt
			if (now - lastTime > 5000)
			{
				status = 2;
				DEBUG_M2( "ButtonState ", status);
			}
		}
		else
		{              // nicht mehr gedrückt
			if (now - lastTime > 100) // determine sensors and send announce on short press
				hmwmodule->broadcastAnnounce(0);
			status = 0;
			DEBUG_M2( "ButtonState ", status);
		}
		;
		break;
	case 2:
		if (!buttonState)
		{  // losgelassen
			status = 3;
			DEBUG_M2( "ButtonState ", status);
			lastTime = now;
		}
		;
		break;
	case 3:
		// wait at least 100ms
		if (now - lastTime < 100)
			break;
		if (buttonState)
		{   // zweiter Tastendruck
			status = 4;
			DEBUG_M2( "ButtonState ", status);
			lastTime = now;
		}
		else
		{              // noch nicht gedrueckt
			if (now - lastTime > 10000)
			{
				status = 0;
				DEBUG_M2( "ButtonState ", status);
			}   // give up
		}
		;
		break;
	case 4:
		if (now - lastTime < 100) // entprellen
			break;
		if (buttonState)
		{   // immer noch gedrueckt
			if (now - lastTime > 3000)
			{
				status = 5;
				DEBUG_M2( "ButtonState ", status);
			}
		}
		else
		{              // nicht mehr gedrückt
			status = 0;
			DEBUG_M2( "ButtonState ", status);
		}
		;
		break;
	case 5:   // zweiter Druck erkannt
		if (!buttonState)
		{    //erst wenn losgelassen
			// Factory-Reset          !!!!!!  TODO: Gehoert das ins Modul?
			factoryReset();
			hmwmodule->setNewId();
			status = 0;
			DEBUG_M2( "ButtonState ", status);
		}
		break;
	}

	// control LED
	static long lastLEDtime = 0;
	switch (status)
	{
	case 0:
		ioport_set_value(LED, LOW);
		break;
	case 1:
		ioport_set_value(LED, HIGH);
		break;
	case 2:
	case 3:
	case 4:
		if (now - lastLEDtime > 100)
		{  // schnelles Blinken
			ioport_set_value(LED, !ioport_get_value(LED));
			lastLEDtime = now;
		}
		;
		break;
	case 5:
		if (now - lastLEDtime > 750)
		{  // langsames Blinken
			ioport_set_value(LED, !ioport_get_value(LED));
			lastLEDtime = now;
		}
		;
	}
}
;

//--------------------------------------------------------

// Tasten
void handleKeys()
{
// TODO: Vielleicht besser eine Klasse HMWKey oder so anlegen
	// millis() zum Zeitpunkt eines Tastendrucks
	// verwendet zum Entprellen, lange Tastendruecke und wiederholtes Senden langer Tastendruecke
	static unsigned long keyPressedMillis[2] =
	{ 0, 0 };   // Wir haben zwei Inputs
	static uint8_t keyPressNum[2] =
	{ 0, 0 };
	// wann wurde das letzte mal "short" gesendet?
	static unsigned long lastSentLong[2] =
	{ 0, 0 };

	SystemTime::time_t now = SystemTime::now();

	for (uint8_t i = 0; i < 2; i++)
	{
// INPUT_LOCKED?
		if (!config.keys[i].input_locked)
			continue;   // inverted logic, locked = 0
// Taste nicht gedrueckt (negative Logik wegen INPUT_PULLUP)
		if ( portStatus[i / 8] & (1<<(i % 8)))
		{
			// Taste war auch vorher nicht gedrueckt kann ignoriert werden
			// Taste war vorher gedrueckt?
			if (keyPressedMillis[i])
			{
				// entprellen, nur senden, wenn laenger als 50ms gedrueckt
				// aber noch kein "long" gesendet
				if (now - keyPressedMillis[i] >= 50 && !lastSentLong[i])
				{
					keyPressNum[i]++;
// TODO: muss das eigentlich an die Zentrale gehen?
					hmwmodule->broadcastKeyEvent(i, keyPressNum[i]);
					// gleich ein Announce hinterher
					// TODO: Vielleicht gehoert das in den allgemeinen Teil
					hmwmodule->broadcastAnnounce(i);
				};
				keyPressedMillis[i] = 0;
			};
		}
		else
		{
// Taste gedrueckt
			// Taste war vorher schon gedrueckt
			if (keyPressedMillis[i])
			{
				// muessen wir ein "long" senden?
				if (lastSentLong[i])
				{   // schon ein LONG gesendet
					if (now - lastSentLong[i] >= 300)
					{  // alle 300ms wiederholen
						// keyPressNum nicht erhoehen
						lastSentLong[i] = now ? now : 1; // der Teufel ist ein Eichhoernchen
						// TODO: muss das eigentlich an die Zentrale gehen?
						hmwmodule->broadcastKeyEvent(i, keyPressNum[i], true);
					};
				}
				else if (SystemTime::now() - keyPressedMillis[i]
						>= long(config.keys[i].long_press_time) * 100)
				{
					// erstes LONG
					keyPressNum[i]++;
					lastSentLong[i] = SystemTime::now();
					// TODO: muss das eigentlich an die Zentrale gehen?
					hmwmodule->broadcastKeyEvent(i, keyPressNum[i], true);
					// gleich ein Announce hinterher
					// TODO: Vielleicht gehoert das in den allgemeinen Teil
					hmwmodule->broadcastAnnounce(i);
				};
			}
			else
			{
				// Taste war vorher nicht gedrueckt
				keyPressedMillis[i] = now ? now : 1; // der Teufel ist ein Eichhoernchen
				lastSentLong[i] = 0;
			}
		}
	}
}

// The loop function is called in an endless loop
void loop()
{

// Check Keys
// Hier werden alle Ein-/Ausgaenge gelesen
// Pins lesen und nach portStatus[] schreiben
	readPins();

// Tasten abfragen, entprellen etc.
	handleKeys();

	// Bedienung ueber Button für Anlernen und Factory Reset
	handleButton();

	// hmwTxTargetAdress(4)                   the target adress
	// hmwTxFrameControllByte                 the controll uint8_t
	// hmwTxSenderAdress(4)                   the sender adress
	// hmwTxFrameDataLength                   the length of data to send
	// hmwTxFrameData(MAX_RX_FRAME_LENGTH)    the data array to send
}

