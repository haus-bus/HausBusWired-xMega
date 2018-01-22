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
      }

      virtual inline bool isConfigButtonPressed()
      {
         // no config button available
         return false;
      }

      virtual inline void notifyConfigButtonState( ConfigButtonState state )
      {
         // no visualization available
      }
   protected:
   private:


}; // HBWIo12OneWireUpHw

#endif // __HBW_IO12_1W_UP_HW_H__
