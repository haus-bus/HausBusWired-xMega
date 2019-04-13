/*
 * IpStackManager.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "IpStackManager.h"
#include "Protocols/Ethernet/ArpHeader.h"
#include "ArpManager.h"
#include "EventPkg/EventPkg.h"
#include "Protocols/Ethernet/IcmpHeader.h"
#include "Scheduler.h"
#include "TcpConnection.h"
#include "Protocols/Ethernet/TcpHeader.h"
#include "UdpConnection.h"
#include "Protocols/Ethernet/UdpHeader.h"

const uint8_t IpStackManager::debugLevel( DEBUG_LEVEL_OFF );

IpStackManager* IpStackManager::theInstance( NULL );

IpStackManager::IpStackManager() :
   linkStatus( false )
{
   theInstance = this;
   setId( ( ClassId::IP_STACK_MANAGER << 8 ) | 1 );
}

bool IpStackManager::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      run();
      setSleepTime( 1 );
   }
   else if ( event.isEvData() )
   {

   }
   return false;
}

void* IpStackManager::operator new( size_t size )
{
   return allocOnce( size );
}

void IpStackManager::pollTcpConnections()
{
   /*
      uint8_t c = 0;
      TcpConnection* connection = TcpConnection::getConnection( c++ );
      while( connection )
      {
      EventDrivenUnit* user = connection->getItsUser();
      if( user )
      {
      do
      {
      transferDescriptor.pData = &buffer[sizeof(TcpHeader)];
      transferDescriptor.bytesTransferred = 0;
      transferDescriptor.bytesRemaining = IP_STACK_BUFFER_SIZE - sizeof(UdpHeader);
      if( user->notifyEvent( Event( user, Event::evData, 0, &transferDescriptor ) ) )
      {
      connection->fillHeader( (TcpHeader*)buffer, transferDescriptor.bytesTransferred );
      ArpManager::prepareOutPacket( *(IpHeader*)buffer );
      stream.write( buffer, sizeof(TcpHeader) + transferDescriptor.bytesTransferred );
      }
      }
      while( transferDescriptor.bytesTransferred );
      }
      connection = TcpConnection::getConnection( c++ );
      }
    */
}

void IpStackManager::checkTcpConnections()
{
   // Increase the initial sequence number.
   TcpConnection::initialSeqNumber++;

   /*
      // Reset the length variables.
      uint16_t dataLen = 0

      // Check if the connection is in a state in which we simply wait
      // for the connection to time out. If so, we increase the
      // connection's timer and remove the connection if it times
      // out.
      if(uip_connr->tcpstateflags == UIP_TIME_WAIT ||
      uip_connr->tcpstateflags == UIP_FIN_WAIT_2)
      {
    ++(uip_connr->timer);
      if(uip_connr->timer == UIP_TIME_WAIT_TIMEOUT)
      {
      uip_connr->tcpstateflags = UIP_CLOSED;
      }
      }
      else if(uip_connr->tcpstateflags != UIP_CLOSED)
      {
      // If the connection has outstanding data, we increase the
      // connection's timer and see if it has reached the RTO value
      // in which case we retransmit.
      if(uip_outstanding(uip_connr))
      {
      if(uip_connr->timer-- == 0)
      {
      if( uip_connr->nrtx == UIP_MAXRTX ||
      ((uip_connr->tcpstateflags == UIP_SYN_SENT ||
      uip_connr->tcpstateflags == UIP_SYN_RCVD) &&
      uip_connr->nrtx == UIP_MAXSYNRTX))
      {
      uip_connr->tcpstateflags = UIP_CLOSED;

      // We call UIP_APPCALL() with uip_flags set to
      // UIP_TIMEDOUT to inform the application that the
      // connection has timed out.
      uip_flags = UIP_TIMEDOUT;
      UIP_APPCALL();

      // We also send a reset packet to the remote host.
      BUF->flags = TCP_RST | TCP_ACK;
      goto tcp_send_nodata;
      }

      // Exponential backoff.
      uip_connr->timer = UIP_RTO << (uip_connr->nrtx > 4?
      4:
      uip_connr->nrtx);
    ++(uip_connr->nrtx);

      // Ok, so we need to retransmit. We do this differently
      // depending on which state we are in. In ESTABLISHED, we
      // call upon the application so that it may prepare the
      // data for the retransmit. In SYN_RCVD, we resend the
      // SYNACK that we sent earlier and in LAST_ACK we have to
      // retransmit our FINACK.
      UIP_STAT(++uip_stat.tcp.rexmit);
      switch(uip_connr->tcpstateflags & UIP_TS_MASK) {
      case UIP_SYN_RCVD:
      // In the SYN_RCVD state, we should retransmit our
      //     SYNACK.
      goto tcp_send_synack;

    #if UIP_ACTIVE_OPEN
      case UIP_SYN_SENT:
      // In the SYN_SENT state, we retransmit out SYN.
      BUF->flags = 0;
      goto tcp_send_syn;
    #endif // UIP_ACTIVE_OPEN

      case UIP_ESTABLISHED:
      // In the ESTABLISHED state, we call upon the application
      // to do the actual retransmit after which we jump into
      // the code for sending out the packet (the apprexmit
      // label).
      uip_flags = UIP_REXMIT;
      UIP_APPCALL();
      goto apprexmit;

      case UIP_FIN_WAIT_1:
      case UIP_CLOSING:
      case UIP_LAST_ACK:
      // In all these states we should retransmit a FINACK.
      goto tcp_send_finack;

      }
      }
      } else if((uip_connr->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED) {
      // If there was no need for a retransmission, we poll the
      // application for new data.
      uip_flags = UIP_POLL;
      UIP_APPCALL();
      goto appsend;
      }
      }
      goto drop;
    */
}

uint16_t IpStackManager::handleIcmpPacket( IcmpHeader* icmp )
{
   DEBUG_M1( FSTR( "icmp: " ) );

   if ( !icmp->isEchoRequest() )
   {
      ERROR_1( FSTR( "not an echo request." ) );
      return 0;
   }

   // If we are configured to use ping IP address assignment, we use
   // the destination IP address of this ping packet and assign it to ourself
   if ( !IP::local.isValid() )
   {
      IP::local = icmp->destinationAddress;
      DEBUG_L1( FSTR( "new IP configured: " ) );
   }

   icmp->setType( IcmpHeader::ECHO_REPLY );
   if ( icmp->checksum >= changeEndianness( (uint16_t) ( 0xFFFF - ( IcmpHeader::ECHO << 8 ) ) ) )
   {
      icmp->checksum += changeEndianness( (uint16_t) ( IcmpHeader::ECHO << 8 ) ) + 1;
   }
   else
   {
      icmp->checksum += changeEndianness( (uint16_t) ( IcmpHeader::ECHO << 8 ) );
   }

   // Swap IP addresses.
   icmp->destinationAddress = icmp->sourceAddress;
   icmp->sourceAddress = IP::local;

   return icmp->getPacketSize();
}

uint16_t IpStackManager::handleTcpPacket( TcpHeader* packet )
{
   DEBUG_M1( FSTR( "tcp: " ) );

   if ( !packet->IpHeader::getChecksum() || packet->isChecksumCorrect() )
   {
      // search for connection
      TcpConnection* connection = TcpConnection::getConnection( packet );
      if ( connection )
      {
         DEBUG_L2( FSTR( " connection: " ), (uintptr_t)connection );
         if ( packet->isRST() )
         {
            DEBUG_M1( FSTR( "got reset, aborting connection" ) );
            connection->reset();
            return 0;
         }
         // First, check if the sequence number of the incoming packet is what we're
         // expecting next. If not, we send out an ACK with the correct numbers in.
         uint16_t dataLength = packet->getPacketSize() - sizeof( TcpHeader )
                               + sizeof( EthernetHeader ) - packet->getDataOffset();
         if ( !connection->inSYN_SENT() || !packet->isSYN_ACK() )
         {
            if ( ( dataLength || packet->isSYN() || packet->isFIN() )
               && ( connection->getReceiveNextSequenceNumber()
                    != packet->getSeqNumber() ) )
            {
               packet->changeToAckPacket();
               connection->fillHeader( packet, 0 );
               return packet->getPacketSize();
            }
         }
         connection->notifyReceivedPacket( packet );
         // TODO

         return packet->getPacketSize();
      }
      else
      {
         DEBUG_M1( FSTR( "no matching connection found" ) );
         // If we didn't find and active connection that expected the packet,
         // either this packet is an old duplicate, or this is a SYN packet
         // destined for a connection in LISTEN. If the SYN flag isn't set,
         // it is an old packet and we send a RST.
         if ( packet->isOnlySYN() )
         {
            // check if we are listening on the destination port
            EventDrivenUnit* user = IpConnection::isListeningToPort(
               packet->getDestinationPort() );
            if ( user )
            {
               DEBUG_M1( FSTR( "listening tcp connection found" ) );
               // First we check if there are any connections avaliable. Unused
               // connections are kept in the same table as used connections, but
               // unused ones have the tcpstate set to CLOSED. Also, connections in
               // TIME_WAIT are kept track of and we'll use the oldest one if no
               // CLOSED connections are found.
               TcpConnection* con = TcpConnection::connect( packet, user );
               if ( con )
               {
                  packet->prepareSynAck( MAX_SEGMENT_SIZE );
                  con->fillHeader( packet, packet->OPT_MSS_LEN );
                  return packet->getPacketSize();
               }
               else
               {
                  ERROR_1( FSTR( "no unused connection available!" ) );
               }
            }
            else
            {
               DEBUG_M1( FSTR( "no listening connection found" ) );
            }
         }
         if ( !packet->isRST() )
         {
            DEBUG_M1( FSTR( "send RST" ) );
            packet->changeToResetPacket();
            TcpConnection::fillNoConnectionHeader( packet );
            return packet->getPacketSize();
         }
      }
   }
   else
   {
      ERROR_1( FSTR( "tcp: bad checksum" ) );
   }
   return 0;
}

uint16_t IpStackManager::handleUdpPacket( UdpHeader* header )
{
   EventDrivenUnit* user = NULL;

   // search for connection
   UdpConnection* connection = UdpConnection::getConnection( header );
   if ( connection )
   {
      user = connection->getItsUser();
   }
   else
   {
      user = IpConnection::isListeningToPort( header->getDestinationPort() );
   }

   if ( user )
   {
      DEBUG_M4( FSTR( "Udp-Datagramm from : " ), header->sourceAddress, ':', header->getSourcePort() );
      DEBUG_M4( FSTR( "Destination : " ), header->destinationAddress, ':', header->getDestinationPort() );

      transferDescriptor.pData = &buffer[sizeof( UdpHeader )];
      transferDescriptor.bytesTransferred = header->getPacketSize()
                                            - sizeof( UdpHeader ) + sizeof( EthernetHeader );
      transferDescriptor.bytesRemaining = 0;
      if ( evData( user, &transferDescriptor ).send() )
      {
         UdpConnection::convertHeaderToResponse( header, transferDescriptor.bytesTransferred );
         return transferDescriptor.bytesTransferred;
      }
   }

   return 0;
}

void IpStackManager::run()
{
   SET_STATE_L1( RUNNING );
   if ( IpConnection::stream == NULL )
   {
      return;
   }

   if ( lastArpUpdate.elapsed( 10 * SystemTime::S ) )
   {
      lastArpUpdate = Timestamp();
      ArpManager::refreshTable();
   }
   if ( lastConnectionCheck.elapsed( 500 * SystemTime::MS ) )
   {
      Reactive* dhcp = Scheduler::getJob( ( ClassId::DHCP << 8 ) | 1 );
      if ( dhcp )
      {
         bool linkedUp = IpConnection::stream->isLinkedUp();
         if ( linkStatus != linkedUp )
         {
            evConnect( dhcp, linkedUp ).send();
            linkStatus = linkedUp;
         }
      }

      lastConnectionCheck = Timestamp();
      checkTcpConnections();
   }

   pollTcpConnections();

   uint16_t dataLength = IpConnection::stream->read( buffer, sizeof( buffer ) );
   if ( dataLength < sizeof( IpHeader ) )
   {
      if ( dataLength != 0 )
      {
         ERROR_1( "packet too small" );
      }
      return;
   }

   EthernetHeader* eth = (EthernetHeader*) buffer;
   if ( eth->isIpDatagramm() )
   {
      DEBUG_H3( FSTR( ".packet received: 0x" ), dataLength, FSTR( " Bytes" ) );

      IpHeader* ipHdr = (IpHeader*) buffer;
      if ( !ipHdr->isValid() || ipHdr->isFragmented() )
      {
         WARN_1( "packet invalid or fragmented" );
         return;
      }
      if ( !ipHdr->isPacketSizeValid( dataLength ) )
      {
         ERROR_1( "packet has invalid size" );
         return;
      }
      if ( ipHdr->getChecksum() && !ipHdr->isChecksumCorrect() )
      {
         ERROR_1( "packet has bad checksum" );
         return;
      }

      ArpManager::notifyIpPacket( ipHdr );

      uint16_t responseLength = 0;
      if ( ipHdr->isProtocollUdp()
         && ( ipHdr->destinationAddress.isBroadcast()
            || ipHdr->destinationAddress.isLocalBroadcast() ) )
      {
         handleUdpPacket( (UdpHeader*) buffer );
      }
      else if ( ipHdr->destinationAddress.isForMe() || ( !IP::local.isValid() && ipHdr->isProtocollIcmp() ) )
      {
         if ( ipHdr->isProtocollUdp() )
         {
            responseLength = handleUdpPacket( (UdpHeader*) buffer );
         }
         else if ( ipHdr->isProtocollTcp() )
         {
            responseLength = handleTcpPacket( (TcpHeader*) buffer );
         }
         else if ( ipHdr->isProtocollIcmp() )
         {
            responseLength = handleIcmpPacket( (IcmpHeader*) buffer );
         }
         else
         {
            ERROR_2( FSTR( "unknown protocol: " ), ipHdr->getProtocol() );
         }
      }
      else
      {
         DEBUG_M1( FSTR( "packet dropped, not for me" ) );
      }
      if ( responseLength )
      {
         ArpManager::prepareOutPacket( (IpHeader*) buffer, responseLength );
         IpConnection::stream->write( buffer, responseLength );
         DEBUG_M1( FSTR( "response sent" ) );
         return;
      }
   }
   else if ( eth->isArp() )
   {
      uint16_t responseLength = ArpManager::notifyArpPacket(
         (ArpHeader*) buffer );
      if ( responseLength )
      {
         IpConnection::stream->write( buffer, responseLength );
      }
   }
}

bool IpStackManager::sendToUdp( uint16_t port, void* pData, uint16_t len, const IP& _remoteIp )
{
   memcpy( &buffer[sizeof( UdpHeader )], pData, len );
   UdpConnection::fillNoConnectionHeader( (UdpHeader*) buffer, len, port, _remoteIp );

   // ArpManager might change the packet into an arp-request, in this case the return value will be false
   bool success = ArpManager::prepareOutPacket( (IpHeader*) buffer, len );
   if ( !IpConnection::stream || ( IpConnection::stream->write( buffer, len ) != len ) )
   {
      return false;
   }
   DEBUG_M1( FSTR( "sendToUdp()" ) );
   return success;
}

IStream::TransferDescriptor* IpStackManager::getTransferDescriptor() const
{
   return (IStream::TransferDescriptor*) &transferDescriptor;
}

