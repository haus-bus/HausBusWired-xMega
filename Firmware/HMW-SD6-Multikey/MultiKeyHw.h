/*
 * MultiKeyHw.h
 *
 * Created: 23.11.2017 22:34:53
 * Author: viktor.pankraz
 */


#ifndef __MULTIKEYHW_H__
#define __MULTIKEYHW_H__

#include <DigitalOutput.h>
#include <HMWired/HmwStreamHw.h>
#include <HMWired/HmwKey.h>
#include <HMWired/HmwDimmer.h>
#include <HMWired/HmwDS1820.h>
#include <HMWired/HmwAnalogIn.h>
#include <HMWired/HmwDevice.h>
#include <HMWired/HmwLinkKey.h>
#include <HMWired/HmwLinkDimmer.h>


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

   protected:
   private:

      DigitalOutput txEnable;

      inline MultiKeyHw( PortPin _txEnable, Usart* _serial ) : txEnable( _txEnable )
      {
         serial = _serial;
      }


}; // MultiKeyHw

#endif // __MULTIKEYHW_H__
