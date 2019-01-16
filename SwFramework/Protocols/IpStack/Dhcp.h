/*
 * Dhcp.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_IpStack_Dhcp_H
#define SwFramework_Protocols_IpStack_Dhcp_H

#include "IpStack.h"
#include "Protocols/Ethernet/IP.h"
#include "Reactive.h"
#include "IStream.h"

class Event;

class MAC;

class Scheduler;

class SystemTime;

class UdpConnection;

class Dhcp : public Reactive
{
   public:

      enum States
      {
         INITIAL,
         SEND_DISCOVER,
         DISCOVER_SENT,
         OFFER_RECEIVED,
         SEND_REQUEST,
         REQUEST_SENT,
         CONFIG_RECEIVED,
         SEND_REFRESH,
         REFRESH_SENT,
         REFRESH_SUCCEEDED,
         NAK_RECEIVED,
         TIMEOUT
      };

      enum Ports
      {
         SERVER_PORT = 67,
         CLIENT_PORT = 68
      };

      enum Options
      {
         OPTION_SUBNET_MASK = 1,
         OPTION_ROUTER = 3,
         OPTION_DNS_SERVER = 6,
         OPTION_REQ_IPADDR = 50,
         OPTION_LEASE_TIME = 51,
         OPTION_MSG_TYPE = 53,
         OPTION_SERVER_ID = 54,
         OPTION_REQ_LIST = 55,
         OPTION_END = 255
      };

      enum MsgType
      {
         DISCOVER = 1,
         OFFER,
         REQUEST,
         DECLINE,
         ACK,
         NAK,
         RELEASE
      };

      struct Message
      {
         uint8_t op;
         uint8_t hType;
         uint8_t hLength;
         uint8_t hops;
         uint32_t xId;
         uint16_t secs;
         uint16_t flags;
         uint32_t ciAddr;
         uint32_t yiAddr;
         uint8_t siAddr[4];
         uint32_t giAddr;
         uint8_t chAddr[16];
         uint8_t sName[64];
         uint8_t file[128];
         uint8_t options[312];
      };

      enum Opcodes
      {
         OPCODE_REQUEST = 1,
         OPCODE_REPLY = 2
      };

      static const uint8_t DHCP_HTYPE_ETHERNET = 1;
      static const uint16_t BOOTP_BROADCAST = 0x0080; // big Indian format
      static const uint32_t xId = 0x2312dead;
      static const uint32_t magicCookie = 0x63538263;

      ////    Constructors and destructors    ////

      Dhcp();

      ////    Operations    ////

      inline static uint8_t* addEnd( uint8_t* ptr );

      static uint8_t* addMessageType( uint8_t* ptr, MsgType type );

      static uint8_t* addRequestIpAddr( uint8_t* ptr );

      static uint8_t* addRequestOptions( uint8_t* ptr );

      inline static uint8_t* addServerId( uint8_t* ptr );

      void createMessage( Message* msg );

      virtual bool notifyEvent( const Event& event );

      void* operator new( size_t size );

      uint8_t parseMessage( const Message& msg, uint8_t* end );

      uint8_t parseOptions( const Message& msg, uint8_t* end );

      void run();

      void sendDiscover();

      void sendRequest();

   private:

      void dumpOfferedSettings();

      ////    Additional operations    ////

   public:

      inline uint32_t getLeaseTime() const
      {
         return leaseTime;
      }

      inline void setLeaseTime( uint32_t p_leaseTime )
      {
         leaseTime = p_leaseTime;
      }

      inline static IP* getOfferedLocalIp()
      {
         return (IP*) &offeredLocalIp;
      }

      inline static IP* getOfferedNetmask()
      {
         return (IP*) &offeredNetmask;
      }

      inline static IP* getOfferedRouterIp()
      {
         return (IP*) &offeredRouterIp;
      }

      inline uint8_t getRetries() const
      {
         return retries;
      }

      inline void setRetries( uint8_t p_retries )
      {
         retries = p_retries;
      }

      inline static IP* getServerIp()
      {
         return (IP*) &serverIp;
      }

   private:

      inline static uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      inline static void setDebugLevel( uint8_t p_debugLevel )
      {
         debugLevel = p_debugLevel;
      }

      ////    Attributes    ////

   public:

      uint32_t leaseTime;

      static IP offeredLocalIp;

      static IP offeredNetmask;

      static IP offeredRouterIp;

      uint8_t retries;

      static IP serverIp;

   private:

      static uint8_t debugLevel;

};

inline uint8_t* Dhcp::addEnd( uint8_t* ptr )
{
   *ptr++ = OPTION_END;
   return ptr;
}

inline uint8_t* Dhcp::addServerId( uint8_t* ptr )
{
   *ptr++ = OPTION_SERVER_ID;
   *ptr++ = 4;
   memcpy( ptr, &serverIp, 4 );
   return ptr + 4;
}

#endif

