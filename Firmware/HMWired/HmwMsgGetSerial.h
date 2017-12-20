/*
 * HmwMsgGetSerial.h
 *
 * Created: 19.11.2017 02:29:51
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGGETSERIAL_H__
#define __HMWMSGGETSERIAL_H__

#include "HmwMessageBase.h"

class HmwMsgGetSerial : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline void setupResponse( uint32_t& ownAddress )
      {
         convertToHmwSerialString( ownAddress, &frameData[0] );
         frameDataLength = 10;
      }

   protected:

   private:

}; // HmwMsgGetSerial

#endif // __HMWMSGGETSERIAL_H__
