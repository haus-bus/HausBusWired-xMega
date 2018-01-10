/*
 * HmwLinkSender.h
 *
 * Created: 19.11.2017 01:21:03
 * Author: viktor.pankraz
 */


#ifndef __HMWLINKSENDER_H__
#define __HMWLINKSENDER_H__

#include <Stream.h>

class HmwLinkSender
{
   protected:

      inline HmwLinkSender()
      {
         instance = this;
      }

   private:

      static HmwLinkSender* instance;

      // functions
   public:
      virtual Stream::Status sendKeyEvent( uint8_t srcChan, uint8_t keyPressNum, bool longPress ) = 0;

      static inline Stream::Status notifyKeyEvent( uint8_t srcChan, uint8_t keyPressNum, bool longPress )
      {
         if ( instance )
         {
            return instance->sendKeyEvent( srcChan, keyPressNum, longPress );
         }
         return Stream::NOT_SUPPORTED;
      }
}; // HmwLinkSender

#endif // __HMWLINKSENDER_H__
