/*
 * HmwLinkReceiver.h
 *
 * Created: 19.11.2017 01:20:39
 * Author: viktor.pankraz
 */


#ifndef __HMWLINKRECEIVER_H__
#define __HMWLINKRECEIVER_H__


class HmwLinkReceiver
{
   public:
      virtual void receiveKeyEvent( const uint32_t& senderAddress, uint8_t senderChannel, uint8_t targetChannel, bool longPress ) = 0;
}; // HmwLinkReceiver

#endif // __HMWLINKRECEIVER_H__
