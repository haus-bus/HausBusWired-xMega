/*
 * HBWMulltiKeySD6v0Hw.h
 *
 * Created: 09.01.2018 18:07:15
 * Author: viktor.pankraz
 */


#ifndef __HBWMULTIKEYSD6V0HW_H__
#define __HBWMULTIKEYSD6V0HW_H__

#include "HBWMultiKeySD6BaseHw.h"

#include <DigitalOutput.h>

class HBWMultiKeySD6v0Hw : public HBWMultiKeySD6BaseHw
{
// variables
   public:

   protected:

   private:

      DigitalOutputTmpl<PortE, 0> rxEnable;

// functions
   public:
      inline HBWMultiKeySD6v0Hw() : HBWMultiKeySD6BaseHw( PortPin( PortE, 1 ), PortPin( PortD, 7 ), true )
      {
         // set ledFeedback channels
         for ( uint8_t i = 0; i < 12; i++ )
         {
            ( (HmwKey*)HmwChannel::getChannel( i ) )->setFeedbackChannel( HmwChannel::getChannel( i + 12 ) );
         }
      }



   protected:
   private:


}; // HBWMultiKeySD6v0Hw

#endif // __HBWMULTIKEYSD6V0HW_H__
