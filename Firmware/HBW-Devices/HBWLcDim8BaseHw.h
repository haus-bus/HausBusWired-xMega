/*
 * HBWMultiKeySD6BaseHw.h
 *
 * Created: 09.01.2018 18:36:42
 * Author: viktor.pankraz
 */


#ifndef __HBWLCDIM8BASEHW_H__
#define __HBWLCDIM8BASEHW_H__

#include "HBWGenericDeviceHw.h"

#include <HMWired/HmwDimmer.h>
#include <PortPin.h>


class HBWLcDim8BaseHw : public HBWGenericDeviceHw
{
// variables
   public:
   protected:

      HmwDimmer hbwDimmer1, hbwDimmer2, hbwDimmer3, hbwDimmer4, hbwDimmer5, hbwDimmer6, hbwDimmer7, hbwDimmer8;

   private:

// functions
   public:

      HBWLcDim8BaseHw();

      HmwDeviceHw::BasicConfig* getBasicConfig();

      virtual inline void enableTranceiver( bool enable )
      {
         enable ? txEnable.set() : txEnable.clear();
      }

      virtual inline bool isConfigButtonPressed()
      {
         return false;
      }

      virtual inline void notifyConfigButtonState( ConfigButtonState state )
      {
         static Timestamp lastLEDtime;
         static DigitalOutputTmpl<PortR, 1> configLed;

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
      
      DigitalOutputTmpl<PortA, 5> txEnable;

      DigitalOutputTmpl<PortA, 6> rxEnable;

}; // HBWLcDim8BaseHw

#endif // __HBWLCDIM8BASEHW_H__
