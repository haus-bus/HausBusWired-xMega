/*
 * HmwLinkSender.h
 *
 * Created: 19.11.2017 01:21:03
 * Author: viktor.pankraz
 */


#ifndef __HMWLINKSENDER_H__
#define __HMWLINKSENDER_H__

#include <IStream.h>

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
      virtual IStream::Status sendKeyEvent( uint8_t srcChan, uint8_t keyPressNum, bool longPress ) = 0;

      static inline IStream::Status notifyKeyEvent( uint8_t srcChan, uint8_t keyPressNum, bool longPress )
      {
         if ( instance )
         {
            return instance->sendKeyEvent( srcChan, keyPressNum, longPress );
         }
         return IStream::NOT_SUPPORTED;
      }
}; // HmwLinkSender

#endif // __HMWLINKSENDER_H__
