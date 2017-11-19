/*
 * HmwLinkSender.h
 *
 * Created: 19.11.2017 01:21:03
 * Author: viktor.pankraz
 */


#ifndef __HMWLINKSENDER_H__
#define __HMWLINKSENDER_H__


class HmwLinkSender
{
   public:
      virtual void sendKeyEvent( uint8_t srcChan, uint8_t keyPressNum, bool longPress ) = 0;
}; // HmwLinkSender

#endif // __HMWLINKSENDER_H__
