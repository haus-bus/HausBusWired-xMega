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
#include "HmwMsgKeyEvent.h"
#include "HmwMsgInfo.h"
#include "HmwLinkReceiver.h"
#include "HmwLinkSender.h"

#include <xEeprom.h>

class HmwDevice
{
// variables
   public:
      struct BasicConfig
      {
         uint8_tx hwVersion;                    // 0x0000
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

      static inline void setup( uint8_t _deviceType, BasicConfig* _config )
      {
         deviceType = _deviceType;
         basicConfig = _config;
         ownAddress = changeEndianness( basicConfig->ownAdress );
         pendingActions.announce = true;
      }

      static inline Stream::Status announce( uint8_t channel = 0 )
      {
         HmwMsgAnnounce msg( channel, ownAddress, deviceType, basicConfig->hwVersion, ( Release::MAJOR << 8 ) | Release::MINOR );
      #ifdef _BOOTER_
         return HmwStreamBase::sendMessage( msg );
      #else
         return HmwStream::sendMessage( msg );
      #endif
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

      static inline uint8_t getLoggingTime()
      {
         return basicConfig->loggingTime;
      }

      static void loop();

      static bool processMessage( HmwMessageBase& msg );

      static void handleAnnouncement();

      static uint8_t get( uint8_t channel, uint8_t* data );

      static void set( uint8_t channel, uint8_t length, uint8_t const* const data );

      static inline void receiveKeyEvent( const uint32_t& senderAddress, uint8_t srcChan, uint8_t dstChan, bool longPress )
      {
         if ( linkReceiver )
         {
            linkReceiver->receiveKeyEvent( senderAddress, srcChan, dstChan, longPress );
         }
      }

      static inline Stream::Status sendKeyEvent( uint8_t srcChan, uint8_t keyPressNum, bool longPress )
      {
         Stream::Status status = sendKeyEvent( srcChan, keyPressNum, longPress, 0xFFFFFFFF, 0 );
         if ( linkSender )
         {
            if ( linkSender->sendKeyEvent( srcChan, keyPressNum, longPress ) != Stream::SUCCESS )
            {
               status = announce( srcChan );
            }
         }
         return status;
      }

      static inline Stream::Status sendKeyEvent( uint8_t srcChan, uint8_t keyPressNum, bool longPress, uint32_t targetAddr, uint8_t targetChan )
      {
         HmwMsgKeyEvent msg( ownAddress, targetAddr, srcChan, targetChan, keyPressNum, longPress );
         return HmwStream::sendMessage( msg );
      }

      static inline uint8_t sendInfoMessage( uint8_t channel, uint8_t length, uint8_t const* const data, uint32_t target_address = 0 )
      {
         HmwMsgInfo msg( ownAddress, target_address ? target_address : changeEndianness( basicConfig->centralAddress ), channel, data, length );
         return HmwStream::sendMessage( msg );
      }

   protected:

   private:

      static void handlePendingActions();

      static void handlePendingInMessages();

}; // HmwDevice

#endif // __HMWDEVICE_H__
