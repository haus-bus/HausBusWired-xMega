/*
 * HmwLinkReceiver.h
 *
 * Created: 19.11.2017 01:20:39
 * Author: viktor.pankraz
 */


#ifndef __HMWLINKRECEIVER_H__
#define __HMWLINKRECEIVER_H__

#include <stdio.h>

class HmwLinkReceiver
{

   protected:

      inline HmwLinkReceiver()
      {
         instance = this;
      }

   private:

      static HmwLinkReceiver* instance;

      // functions
   public:
      virtual void receiveKeyEvent( const uint32_t& senderAddress, uint8_t senderChannel, uint8_t targetChannel, bool longPress, uint8_t keyNum = 0 ) = 0;

      static inline void notifyKeyEvent( const uint32_t& senderAddress, uint8_t senderChannel, uint8_t targetChannel, bool longPress, uint8_t keyNum = 0 )
      {
         if ( instance )
         {
            instance->receiveKeyEvent( senderAddress, senderChannel, targetChannel, longPress, keyNum );
         }
      }
}; // HmwLinkReceiver

#endif // __HMWLINKRECEIVER_H__
