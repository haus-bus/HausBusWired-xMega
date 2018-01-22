/*
 * HBWLC4IN4DRHw.h
 *
 * Created: 20.01.2018 11:09:24
 * Author: viktor.pankraz
 */


#ifndef __HBWLC4IN4DRHW_H__
#define __HBWLC4IN4DRHW_H__

#include "HBWGenericDeviceHw.h"

#include <HMWired/HmwKey.h>
#include <HMWired/HmwDimmer.h>
#include <HMWired/HmwLinkKey.h>
#include <HMWired/HmwLinkDimmer.h>

class HBWLC4IN4DRHw : public HBWGenericDeviceHw
{
// variables
   public:

      static const uint8_t DEFAULT_PERIOD_MULTIPLIER = 192;

   protected:

      HmwKey key1, key2, key3, key4;

      HmwDimmer lightControl1, lightControl2, lightControl3, lightControl4;

      HmwLinkKey linkSender;

      HmwLinkDimmer linkReceiver;

   private:

      DigitalOutputTmpl<PortR, 0> txEnable;

      DigitalOutputTmpl<PortR, 1> configLed;

      DigitalInputTmpl<PortA, 4> configbutton;

// functions
   public:
      HBWLC4IN4DRHw();

      HmwDeviceHw::BasicConfig* getBasicConfig();

      void enableTranceiver( bool enable );

      void notifyConfigButtonState( ConfigButtonState state );

      inline bool isConfigButtonPressed()
      {
         return !configbutton.isSet();
      }

   protected:
   private:


}; // HBWLC4IN4DRHw

#endif // __HBWLC4IN4DRHW_H__
