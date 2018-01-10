/*
 * HmwMultiKeySD6BaseHw.h
 *
 * Created: 09.01.2018 18:36:42
 * Author: viktor.pankraz
 */


#ifndef __HMWMULTIKEYSD6BASEHW_H__
#define __HMWMULTIKEYSD6BASEHW_H__

#include <HMWired/HmwDeviceHw.h>
#include <HMWired/HmwKey.h>
#include <HMWired/HmwDimmer.h>
#include <HMWired/HmwDS1820.h>
#include <HMWired/HmwLinkKey.h>
#include <HMWired/HmwLinkDimmer.h>
#include <HMWired/HmwAnalogIn.h>
#include <HMWired/HmwDevice.h>
#include <PortPin.h>


class HmwMultiKeySD6BaseHw : public HmwDeviceHw
{
// variables
   public:
   protected:

      HmwKey hbwKey1, hbwKey2, hbwKey3, hbwKey4, hbwKey5, hbwKey6;
      HmwKey extHbwKey1, extHbwKey2, extHbwKey3, extHbwKey4, extHbwKey5, extHbwKey6;

      HmwDimmer hbwLed1, hbwLed2, hbwLed3, hbwLed4, hbwLed5, hbwLed6;
      HmwDimmer extHbwLed1, extHbwLed2, extHbwLed3, extHbwLed4, extHbwLed5, extHbwLed6;

      OneWire ow;
      HmwDS1820 hbwTmp1, hbwTmp2, hbwTmp3, hbwTmp4, hbwTmp5, hbwTmp6;

      HmwLinkKey linkSender;

      HmwLinkDimmer linkReceiver;

   private:

      DigitalOutput txEnable;

// functions
   public:

      static uint8_t getVersion();

      HmwDeviceHw::BasicConfig* getBasicConfig();

      void enableTranceiver( bool enable );

      virtual inline bool isConfigButtonPressed()
      {
         return hbwKey5.isPressed() && hbwKey6.isPressed();
      }

      virtual inline void notifyConfigButtonState( ConfigButtonState state )
      {
         static ConfigButtonState lastState = IDLE;

         if ( state == lastState )
         {
            return;
         }

         switch ( state )
         {
            case IDLE:
            {
               uint8_t data[] = { 0 };
               hbwLed5.set( sizeof( data ), data );
               hbwLed6.set( sizeof( data ), data );
               hbwKey5.setUnlocked( true );
               hbwKey6.setUnlocked( true );
               break;
            }

            case FIRST_PRESS:
            {
               uint8_t data[] = { HmwDimmer::MAX_LEVEL / 2 };
               hbwLed5.set( sizeof( data ), data );
               hbwLed6.set( sizeof( data ), data );
               hbwKey5.setUnlocked( false );
               hbwKey6.setUnlocked( false );
               break;
            }

            case FIRST_LONG_PRESS:
            {
               uint8_t data[] = { HmwChannel::BLINK_ON, 0, HmwDimmer::MAX_LEVEL, 6, 6, 0xFF };
               hbwLed5.set( sizeof( data ), data );
               hbwLed6.set( sizeof( data ), data );
               break;
            }

            case SECOND_LONG_PRESS:
            {
               uint8_t data[] = { HmwChannel::BLINK_ON, 0, HmwDimmer::MAX_LEVEL, 2, 2, 0xFF };
               hbwLed5.set( sizeof( data ), data );
               hbwLed6.set( sizeof( data ), data );
               break;
            }
            default:
            {
               // nothing to do
            }
         }
         lastState = state;
      }

   protected:

      HmwMultiKeySD6BaseHw( PortPin txEnablePin, PortPin owPin );

   private:

}; // HmwMultiKeySD6BaseHw

#endif // __HMWMULTIKEYSD6BASEHW_H__
