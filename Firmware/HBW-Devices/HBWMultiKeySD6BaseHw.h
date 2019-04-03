/*
 * HBWMultiKeySD6BaseHw.h
 *
 * Created: 09.01.2018 18:36:42
 * Author: viktor.pankraz
 */


#ifndef __HBWMULTIKEYSD6BASEHW_H__
#define __HBWMULTIKEYSD6BASEHW_H__

#include "HBWGenericDeviceHw.h"

#include <HmwUnits/HmwKey.h>
#include <HmwUnits/HmwLed.h>
#include <HmwUnits/HmwDS1820.h>
#include <HmwUnits/HmwSHTC3.h>
#include <HmwUnits/HmwLinkKey.h>
#include <HmwUnits/HmwLinkLed.h>
#include <HmwUnits/HmwBrightness.h>
#include <PortPin.h>


class HBWMultiKeySD6BaseHw : public HBWGenericDeviceHw
{
// variables
   public:
   protected:

      HmwKey hbwKey1, hbwKey2, hbwKey3, hbwKey4, hbwKey5, hbwKey6;
      HmwKey extHbwKey1, extHbwKey2, extHbwKey3, extHbwKey4, extHbwKey5, extHbwKey6;

      HmwLed hbwLed1, hbwLed2, hbwLed3, hbwLed4, hbwLed5, hbwLed6;

      HmwLed extHbwLed1, extHbwLed2, extHbwLed3, extHbwLed4, extHbwLed5, extHbwLed6;

      OneWire ow;
      HmwDS1820 hbwTmp1, hbwTmp2, hbwTmp3, hbwTmp4, hbwTmp5, hbwTmp6;

      HmwBrightness hbwOnboardBrightness;

      HmwSHTC3 shtc3;

      HmwLinkKey linkSender;

      HmwLinkLed linkReceiver;

   private:

      DigitalOutput txEnable;

// functions
   public:

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
               uint8_t data[] = { HmwLed::MAX_LEVEL / 2 };
               hbwLed5.set( sizeof( data ), data );
               hbwLed6.set( sizeof( data ), data );
               hbwKey5.setUnlocked( false );
               hbwKey6.setUnlocked( false );
               break;
            }

            case FIRST_LONG_PRESS:
            {
               uint8_t data[] = { HmwChannel::BLINK_ON, 0, HmwLed::MAX_LEVEL, 5, 5, 0xFF };
               hbwLed5.set( sizeof( data ), data );
               hbwLed6.set( sizeof( data ), data );
               break;
            }

            case SECOND_LONG_PRESS:
            {
               uint8_t data[] = { HmwChannel::BLINK_ON, 0, HmwLed::MAX_LEVEL, 2, 2, 0xFF };
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

      HBWMultiKeySD6BaseHw( PortPin txEnablePin, PortPin owPin, bool invertLed1To6 );

   private:

}; // HBWMultiKeySD6BaseHw

#endif // __HBWMULTIKEYSD6BASEHW_H__
