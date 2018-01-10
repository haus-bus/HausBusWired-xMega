/*
 * HmwMulltiKeySD6v2Hw.h
 *
 * Created: 09.01.2018 18:07:15
 * Author: viktor.pankraz
 */


#ifndef __HMWMULTIKEYSD6V2HW_H__
#define __HMWMULTIKEYSD6V2HW_H__

#include "HmwMultiKeySD6BaseHw.h"

#include <DigitalOutput.h>

class HmwMultiKeySD6v2Hw : public HmwMultiKeySD6BaseHw
{
// variables
   public:

   protected:

   private:

// functions
   public:
      inline HmwMultiKeySD6v2Hw() : HmwMultiKeySD6BaseHw( PortPin( PortR, 0 ), PortPin( PortR, 1 ) )
      {
      }

   protected:
   private:


}; // HmwMultiKeySD6v2Hw

#endif // __HMWMULTIKEYSD6V2HW_H__
