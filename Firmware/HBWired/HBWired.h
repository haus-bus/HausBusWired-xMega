/*
 * HBWired.h
 *
 *  Created on: 19.11.2016
 *      Author: Thorsten Pferdekaemper
 */

#ifndef HBWired_h
#define HBWired_h

#include <stdlib.h>
#include <xEeprom.h>
#include <Time/Timestamp.h>
#include <IoStream.h>
#include <DigitalOutput.h>

#ifndef MAX_CHANNELS
#define MAX_CHANNELS 34
#endif

class HBWDevice;

// Basisklasse fuer Channels
class HBWChannel
{
   public:
      enum Commands
      {
         ON = 201,
         OFF,
         TOGGLE,
         BLINK_ON,
         BLINK_TOGGLE,
         KEY_FEEDBACK_ON,
         KEY_FEEDBACK_OFF
      };

      enum Type
      {
         UNKNOWN,
         HBW_KEY,
         HBW_DIMMER,
         HBW_DS18X20
      };

   private:
      static uint8_t numChannels;
      static HBWChannel* instances[MAX_CHANNELS];

   protected:

      Type type;
      HBWChannel();

   public:

      static inline uint8_t getNumChannels()
      {
         return numChannels;
      }
      static inline HBWChannel* getChannel( uint8_t channel )
      {
         return instances[channel];
      }

      inline bool isOfType( Type _type )
      {
         return type == _type;
      }

      virtual void set( HBWDevice*, uint8_t length, uint8_t const* const data );
      virtual uint8_t get( uint8_t* data ); // returns length, data must be big enough
      virtual void loop( HBWDevice*, uint8_t channel ); // channel for feedbacks etc.
      virtual void checkConfig();
};


class HBWLinkSender
{
   public:
      virtual void sendKeyEvent( HBWDevice* device, uint8_t srcChan,
                                 uint8_t keyPressNum, bool longPress ) = 0;
};


class HBWLinkReceiver
{
   public:
      virtual void receiveKeyEvent( HBWDevice* device, uint32_t senderAddress, uint8_t senderChannel,
                                    uint8_t targetChannel, bool longPress ) = 0;
};


// Events to be processed by the device
// - Read Config: (re-)Read Config selbst ist immer gleich,
// aber ggf. Device-spezifische "nacharbeiten"
// - processKeyEvent: (i.e. incoming 0x4B or 0xCB) For peerings, always module specific so far
// Unclear, whether 0x4B and 0xCB should really be handled differently
// - processGetLevel: (S) Normalerweise geht das an den Kanal, aber spezielle
// "virtuelle" Kanaele sollten erlaubt sein -> get()
// - processSetLock: (l) Aehnlich wie S, aber weniger wichtig. Moeglicherweise immer ok,
// das an den Kanal zu geben (bzw. es ist eh sinnvoller, das uebers
// EEPROM zu machen) -> gar nicht unterstuetzen
// - processSetLevel: (s,x) Siehe Siehe S -> set()

// Commands triggered through the device
// - Key Event 0x4B (Broadcast/Central Address/Peers)
// - Info-Message 0x69 (Broadcast/Central Address. Peers unclear )

#define MAX_RX_FRAME_LENGTH 64


// The HBWired-Device
class HBWDevice
{
   public:

      struct BasicConfig
      {
         uint8_tx doNotUse;                  // 0x0000
         uint8_tx loggingTime;               // 0x0001
         uint32_tx centralAddress;           // 0x0002   - 0x0005
         uint32_tx ownAdress;                // 0x0006   - 0x0009
      };

      HBWDevice( uint8_t _devicetype, uint8_t _hardware_version, uint16_t _firmware_version,
                 IoStream* _rs485, PortPin _txen,
                 BasicConfig* _basicConfig, HBWLinkSender* = NULL, HBWLinkReceiver* = NULL );

      void setConfigPins( PortPin _configPin0, PortPin _configPin1, PortPin _ledPin );

      virtual void loop(); // needs to be called as often as possible

      // get logging time
      uint8_t getLoggingTime();

      virtual void set( uint8_t, uint8_t, uint8_t const* const );   // channel, data length, data
      virtual uint8_t get( uint8_t channel, uint8_t* data );  // returns length
      virtual void checkConfig();

      // target_address = 0 sends info message to central address
      // sendInfoMessage returns...
      // 0 -> ok
      // 1 -> bus not idle
      // 2 -> missing ACK (three times)
      virtual uint8_t sendInfoMessage( uint8_t channel, uint8_t length, uint8_t const* const data, uint32_t target_address = 0 );
      // Allgemeiner "Key Event"
      virtual uint8_t sendKeyEvent( uint8_t channel, uint8_t keyPressNum, bool longPress );
      // Key Event Routine mit Target fuer LinkSender
      virtual uint8_t sendKeyEvent( uint8_t channel, uint8_t keyPressNum, bool longPress,
                                    uint32_t target_address, uint8_t target_channel );
      // Key-Event senden mit Geraetespezifischen Daten (nur Broadcast)
      virtual uint8_t sendKeyEvent( uint8_t srcChan, uint8_t length, void* data );

      virtual void receiveKeyEvent( uint32_t senderAddress, uint8_t srcChan,
                                    uint8_t dstChan, bool longPress );
      virtual void processEvent( uint8_t const* const frameData, uint8_t frameDataLength,
                                 bool isBroadcast = false );

      uint32_t getOwnAddress() const;

   private:
      static const uint8_t debugLevel;

      bool announced;

      HBWLinkSender* linkSender;
      HBWLinkReceiver* linkReceiver;

      uint32_t ownAdress;

      // pins of config button and config LED
      DigitalInput configPin0, configPin1;
      DigitalOutput ledPin;

      // sendFrame macht ggf. Wiederholungen
      // onlyIfIdle: If this is set, then the bus must have been idle since 210+rand(0..100) ms
      // sendFrame returns...
      // 0 -> ok
      // 1 -> bus not idle (only if onlyIfIdle)
      // 2 -> three times no ACK (cannot occur for broadcasts or ACKs)
      uint8_t sendFrame( bool onlyIfIdle = false );
      void sendAck();    // ACK fuer gerade verarbeitete Message senden

      // eigene Adresse setzen und damit auch random seed
      void setOwnAddress( uint32_t address );

      // get central address
      uint32_t getCentralAddress() const;
      void handleBroadcastAnnounce();

      // Senderadresse beim Empfangen
      uint32_t senderAddress;
      // Senden
      uint32_t txTargetAddress;          // Adresse des Moduls, zu dem gesendet wird

      uint8_t txFrameControlByte;
      uint8_t txFrameDataLength;            // Laenge der Daten + Checksum
      uint8_t txFrameData[MAX_RX_FRAME_LENGTH];


      // the broadcast methods return...
      // 0 -> everything ok
      // 1 -> nothing sent because bus busy
      uint8_t broadcastAnnounce( uint8_t );  // channel

      uint8_t deviceType;

      BasicConfig* basicConfig;          // pointer to config object

      uint8_t hardware_version;
      uint16_t firmware_version;

// Das eigentliche RS485-Interface, kann z.B. HBWSoftwareSerial oder (Hardware)Serial sein
      IoStream* serial;
      DigitalOutput txEnablePin;
      // Empfangs-Status
      uint8_t frameStatus;
// Empfangene Daten
      // Empfangen
      uint8_t frameComplete;
      uint32_t targetAddress;
      uint8_t frameDataLength;                   // Laenge der Daten
      uint8_t frameData[MAX_RX_FRAME_LENGTH];
      uint8_t frameControlByte;
// carrier sense
// last time we have received anything
      Timestamp lastReceivedTime;
// current minimum idle time
// will be initialized in constructor
      uint16_t minIdleTime;

      inline void calculateMinIdleTime()
      {
         // bus must be idle 210 + rand(0..100) ms
         static const uint8_t MIN_IDLE_TIME = 210;
         static const uint8_t MAX_RANDOM_IDLE_TIME = 100;
         minIdleTime = MIN_IDLE_TIME + ( random() % MAX_RANDOM_IDLE_TIME );
      }

      void receive();    // wird zyklisch aufgerufen
      bool parseFrame();
      void sendFrameSingle();
      void sendFrameByte( uint8_t, uint16_t* crc = NULL );
      void crc16Shift( uint8_t, uint16_t* );

      void determineSerial( uint8_t* );

      void processEventKey( uint32_t senderAddress, uint8_t const* const frameData );
      void processEventSetLevel( uint8_t channel, uint8_t dataLength, uint8_t const* const data ); // TODO: rename?
      void processEventGetLevel( uint8_t channel, uint8_t command );
      void processEventSetLock();
      void processEmessage( uint8_t const* const frameData );

      void factoryReset();
      void handleConfigButton();

};


#endif /* HBWired_h */
