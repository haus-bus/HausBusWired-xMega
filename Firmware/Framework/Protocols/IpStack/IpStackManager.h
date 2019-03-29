/*
 * IpStackManager.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_IpStack_IpStackManager_H
#define SwFramework_Protocols_IpStack_IpStackManager_H

#include "IpStack.h"
#include <Protocols/Ethernet/IP.h>
#include "Reactive.h"
#include "IStream.h"
#include "Time/Timestamp.h"


class ArpHeader;

class ArpManager;

class Event;

class IcmpHeader;

class Scheduler;

class TcpConnection;

class TcpHeader;

class UdpConnection;

class UdpHeader;

class IpStackManager : public Reactive
{
   ////    Constructors and destructors    ////

   public:

      IpStackManager();

      ////    Operations    ////

      static IpStackManager* instance()
      {
         return theInstance;
      }

      virtual bool notifyEvent( const Event& event );

      void* operator new( size_t size );

      void pollTcpConnections();

      void pollUdpConnections();

      bool sendToUdp( uint16_t port, void* pData, uint16_t len,
                      const IP& _remoteIp = IP::broadcast() );

   private:

      void checkTcpConnections();

      // ICMP echo (i.e., ping) processing. This is simple, we only change the ICMP type from
      // ECHO to ECHO_REPLY and adjust the ICMP checksum before we return the packet.
      uint16_t handleIcmpPacket( IcmpHeader* icmp );

      uint16_t handleTcpPacket( TcpHeader* packet );

      uint16_t handleUdpPacket( UdpHeader* header );

      void run();

      ////    Additional operations    ////

   public:

      IStream::TransferDescriptor* getTransferDescriptor() const;

   protected:

   private:

      inline uint8_t getBuffer( int i1 ) const
      {
         return buffer[i1];
      }

      inline void setBuffer( int i1, uint8_t p_buffer )
      {
         buffer[i1] = p_buffer;
      }

      inline Timestamp* getLastArpUpdate() const
      {
         return (Timestamp*) &lastArpUpdate;
      }

      inline Timestamp* getLastConnectionCheck() const
      {
         return (Timestamp*) &lastConnectionCheck;
      }

      ////    Attributes    ////

   protected:

      static const uint8_t debugLevel;

      static IpStackManager* theInstance;

   private:

      uint8_t buffer[IP_STACK_BUFFER_SIZE];

      Timestamp lastArpUpdate;

      Timestamp lastConnectionCheck;

      bool linkStatus;

      ////    Relations and components    ////

   protected:

      IStream::TransferDescriptor transferDescriptor;
};

#endif
