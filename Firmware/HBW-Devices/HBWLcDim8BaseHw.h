/*
 * HBWMultiKeySD6BaseHw.h
 *
 * Created: 09.01.2018 18:36:42
 * Author: viktor.pankraz
 */


#ifndef __HBWLCDIM8BASEHW_H__
#define __HBWLCDIM8BASEHW_H__

#include "HBWGenericDeviceHw.h"

#include <HmwUnits/HmwDimmer.h>
#include <HmwUnits/HmwLinkDimmer.h>
#include <HmwUnits/HmwDS1820.h>
#include <PortPin.h>


class HBWLcDim8BaseHw : public HBWGenericDeviceHw
{
// variables
   public:

   protected:

      HmwDimmer hbwDimmer1, hbwDimmer2, hbwDimmer3, hbwDimmer4, hbwDimmer5, hbwDimmer6, hbwDimmer7, hbwDimmer8;
      OneWire ow;
      HmwDS1820 hbwTmp1, hbwTmp2, hbwTmp3, hbwTmp4, hbwTmp5, hbwTmp6;

   private:

      static const uint16_t ZCD_DEFAULT_PERIOD = 41000;

      DigitalOutputTmpl<PortA, 5> txEnable;
      DigitalOutputTmpl<PortA, 6> rxEnable;
      DigitalOutputTmpl<PortR, 1> greenLed;
      DigitalOutputTmpl<PortR, 0> configLed;
      DigitalInputTmpl<PortE, 7> configButton;
      HmwLinkDimmer linkReceiver;

// functions
   public:

      HBWLcDim8BaseHw();

      HmwDeviceHw::BasicConfig* getBasicConfig();

      virtual inline void enableTranceiver( bool enable )
      {
         enable ? txEnable.set() : txEnable.clear();
         enable ? greenLed.set() : greenLed.clear();
      }

      virtual inline bool isConfigButtonPressed()
      {
         return !configButton.isSet();
      }

      virtual inline void notifyConfigButtonState( ConfigButtonState state )
      {
         static Timestamp lastLEDtime;

         switch ( state )
         {
            case IDLE:
            {
               configLed.clear();
               break;
            }

            case FIRST_PRESS:
            {
               configLed.set();
               break;
            }

            case FIRST_LONG_PRESS:
            case WAIT_SECOND_PRESS:
            case SECOND_PRESS:
            {
               if ( lastLEDtime.since() > 500 )
               {
                  configLed.toggle();
                  lastLEDtime = Timestamp();
               }
               break;
            }

            case SECOND_LONG_PRESS:
            {
               if ( lastLEDtime.since() > 200 )
               {
                  configLed.toggle();
                  lastLEDtime = Timestamp();
               }
               break;
            }
            default:
            {
               // nothing to do
            }
         }
      }

   protected:

   private:

      void configureZeroCrossDetection();

}; // HBWLcDim8BaseHw

#endif // __HBWLCDIM8BASEHW_H__
