/*
 * HmwMulltiKeySD6v1Hw.h
 *
 * Created: 09.01.2018 18:07:15
 * Author: viktor.pankraz
 */


#ifndef __HMWMULTIKEYSD6V1HW_H__
#define __HMWMULTIKEYSD6V1HW_H__

#include "HmwMultiKeySD6BaseHw.h"

#include <DigitalOutput.h>

class HmwMultiKeySD6v1Hw : public HmwMultiKeySD6BaseHw
{
// variables
   public:

   protected:

   private:

// functions
   public:
      inline HmwMultiKeySD6v1Hw() : HmwMultiKeySD6BaseHw( PortPin( PortR, 0 ), PortPin( PortR, 1 ) )
      {
         // set ledFeedback channels
         for ( uint8_t i = 0; i < 12; i++ )
         {
            ( (HmwKey*)HmwChannel::getChannel( i ) )->setFeedbackChannel( HmwChannel::getChannel( i + 12 ) );
         }
      }

   protected:
   private:


}; // HmwMultiKeySD6v1Hw

#endif // __HMWMULTIKEYSD6V1HW_H__
