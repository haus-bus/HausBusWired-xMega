/*
 * HmwMulltiKeySD6v0Hw.h
 *
 * Created: 09.01.2018 18:07:15
 * Author: viktor.pankraz
 */


#ifndef __HMWMULTIKEYSD6V0HW_H__
#define __HMWMULTIKEYSD6V0HW_H__

#include "HmwMultiKeySD6BaseHw.h"

#include <DigitalOutput.h>

class HmwMultiKeySD6v0Hw : public HmwMultiKeySD6BaseHw
{
// variables
   public:

   protected:

   private:

      DigitalOutputTmpl<PortE, 0> rxEnable;

// functions
   public:
      inline HmwMultiKeySD6v0Hw() : HmwMultiKeySD6BaseHw( PortPin( PortE, 1 ), PortPin( PortD, 7 ) )
      {
         // set ledFeedback channels
         for ( uint8_t i = 0; i < 12; i++ )
         {
            ( (HmwKey*)HmwChannel::getChannel( i ) )->setFeedbackChannel( HmwChannel::getChannel( i + 12 ) );
         }
      }



   protected:
   private:


}; // HmwMultiKeySD6v0Hw

#endif // __HMWMULTIKEYSD6V0HW_H__
