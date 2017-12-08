/*
 * MultiKeyHw.h
 *
 * Created: 23.11.2017 22:34:53
 * Author: viktor.pankraz
 */


#ifndef __MULTIKEYHW_H__
#define __MULTIKEYHW_H__

#include <DigitalOutput.h>
#include <HBWired/HmwStreamHw.h>
#include <HBWired/HmwKey.h>
#include <HBWired/HmwDimmer.h>
#include <HBWired/HmwDS1820.h>
#include <HBWired/HmwAnalogIn.h>
#include <HBWired/HmwDevice.h>
#include <HBWired/HmwLinkKey.h>
#include <HBWired/HmwLinkDimmer.h>


class MultiKeyHw : public HmwStreamHw
{
// variables
   public:
   protected:
   private:

// functions
   public:

      void enableTranceiver( bool enable );

      static MultiKeyHw* create();

      static HmwDevice::BasicConfig* getBasicConfig();

      static HmwLinkReceiver* getLinkReceiver();

      static HmwLinkSender* getLinkSender();

      static void debug( char c );

      static inline HmwStream::MessageQueue* getInMessageQueue()
      {
         static HmwStream::MessageQueue inMessageQueue;
         return &inMessageQueue;
      }

   protected:
   private:

      DigitalOutput txEnable;

      inline MultiKeyHw( PortPin _txEnable, Usart* _serial ) : txEnable( _txEnable )
      {
         serial = _serial;
      }


}; // MultiKeyHw

#endif // __MULTIKEYHW_H__
