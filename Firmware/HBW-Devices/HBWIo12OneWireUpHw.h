/*
 * HBWIo12OneWireUpHw.h
 *
 * Created: 09.01.2018 18:07:15
 * Author: viktor.pankraz
 */


#ifndef __HBW_IO12_1W_UP_HW_H__
#define __HBW_IO12_1W_UP_HW_H__

#include "HBWMultiKeySD6BaseHw.h"

#include <DigitalOutput.h>

class HBWIo12OneWireUpHw : public HBWMultiKeySD6BaseHw
{
// variables
   public:

   protected:

   private:

// functions
   public:
      inline HBWIo12OneWireUpHw() : HBWMultiKeySD6BaseHw( PortPin( PortR, 0 ), PortPin( PortR, 1 ), false )
      {
         configLed.setInverted( true );
         configbutton.enablePullup();
      }

      virtual inline bool isConfigButtonPressed()
      {
         return !configbutton.isSet();
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
      DigitalOutputTmpl<PortA, 6> configLed;

      DigitalInputTmpl<PortA, 7> configbutton;


}; // HBWIo12OneWireUpHw

#endif // __HBW_IO12_1W_UP_HW_H__
