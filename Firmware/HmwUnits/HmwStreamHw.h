/*
 * HmwStreamHw.h
 *
 * Created: 23.11.2017 19:09:46
 * Author: viktor.pankraz
 */


#ifndef __HMWSTREAMHW_H__
#define __HMWSTREAMHW_H__

#include <Peripherals/Usart.h>

class HmwStreamHw
{
// variables
   public:
      Usart* serial;

// functions
   public:
      virtual void enableTranceiver( bool enable ) = 0;


}; // HmwStramHw

#endif // __HMWSTRAMHW_H__
