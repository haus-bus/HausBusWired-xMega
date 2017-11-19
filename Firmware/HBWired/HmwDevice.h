/*
 * HmwDevice.h
 *
 * Created: 06.10.2017 22:40:44
 * Author: viktor.pankraz
 */


#ifndef __HMWDEVICE_H__
#define __HMWDEVICE_H__

#include "HmwStream.h"
#include "HmwMsgAnnounce.h"
#include "HmwLinkReceiver.h"
#include "HmwLinkSender.h"

#include <xEeprom.h>

class HmwDevice
{
// variables
   public:
      struct BasicConfig
      {
         uint8_tx doNotUse;                     // 0x0000
         uint8_tx loggingTime;                  // 0x0001
         uint32_tx centralAddress;              // 0x0002   - 0x0005
         uint32_tx ownAdress;                   // 0x0006   - 0x0009
      };

      struct PendingActions
      {
         uint8_t readConfig : 1;
         uint8_t announce : 1;
         uint8_t resetSystem : 1;
         uint8_t startBooter : 1;
      };

      static uint8_t deviceType;

      static uint8_t hardwareVersion;

      static uint16_t firmwareVersion;

      static BasicConfig* basicConfig;

      static uint32_t ownAddress;

      static PendingActions pendingActions;

      static HmwLinkReceiver* linkReceiver;

      static HmwLinkSender* linkSender;

   protected:

   private:

      static const uint8_t debugLevel;


// functions
   public:
      static inline HmwDevice& instance()
      {
         static HmwDevice hmwDevice;
         return hmwDevice;
      }

      static inline Stream::Status announce()
      {
         HmwMsgAnnounce msg( 0, ownAddress, deviceType, hardwareVersion, firmwareVersion );
         return HmwStream::sendMessage( &msg );
      }

      static inline bool isReadConfigPending()
      {
         return pendingActions.readConfig;
      }

      static inline void clearPendingReadConfig()
      {
         pendingActions.readConfig = false;
      }

      static inline bool isAnnouncementPending()
      {
         return pendingActions.announce;
      }

      static inline void clearPendingAnnouncement()
      {
         pendingActions.announce = false;
      }

      static void loop();

      static bool processMessage( HmwMessageBase* msg );

      static void handleAnnouncement();

   protected:

      static uint8_t get( uint8_t channel, uint8_t* data );

      static void set( uint8_t channel, uint8_t length, uint8_t const* const data );

   private:

      static void handlePendingActions();

}; // HmwDevice

#endif // __HMWDEVICE_H__
