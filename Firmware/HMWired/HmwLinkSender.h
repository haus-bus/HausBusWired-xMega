/*
 * HmwLinkSender.h
 *
 * Created: 19.11.2017 01:21:03
 * Author: viktor.pankraz
 */


#ifndef __HMWLINKSENDER_H__
#define __HMWLINKSENDER_H__

#include <Stream.h>

class HmwLinkSender
{
   public:
      virtual Stream::Status sendKeyEvent( uint8_t srcChan, uint8_t keyPressNum, bool longPress ) = 0;
}; // HmwLinkSender

#endif // __HMWLINKSENDER_H__
