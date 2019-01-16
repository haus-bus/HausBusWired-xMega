/*
 * UdpConnection.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "UdpConnection.h"
#include "ArpManager.h"
#include "Protocols/Ethernet/UdpHeader.h"

UdpConnection UdpConnection::connections[UdpConnection::MAX_UDP_CONNECTIONS];

UdpConnection* UdpConnection::connect( const IP& _remoteIp,
                                       uint16_t _remotePort,
                                       uint16_t _localPort,
                                       EventDrivenUnit* _user )
{
   uint8_t c = MAX_UDP_CONNECTIONS;
   while ( c-- )
   {
      if ( !connections[c].isConnectionUsed() )
      {
         connections[c].setItsUser( _user );
         connections[c].setRemoteIp( _remoteIp );
         connections[c].setRemotePort( _remotePort );
         connections[c].setLocalPort( _localPort );
         return &connections[c];
      }
   }
   return NULL;
}

UdpConnection* UdpConnection::getConnection( UdpHeader* packet )
{
   uint8_t c = MAX_UDP_CONNECTIONS;
   while ( c-- )
   {
      UdpConnection* con = &connections[c];
      if ( con->getItsUser() != 0 )
      {
         if ( con->localPort == packet->getDestinationPort() )
         {
            if ( con->remoteIp.isBroadcast() || con->remoteIp.isLocalBroadcast() || ( con->remoteIp == packet->sourceAddress ) )
            {
               return con;
            }
         }
      }
   }
   return 0;
}

void UdpConnection::fillNoConnectionHeader( UdpHeader* pHdr,
                                            uint16_t dataLength,
                                            uint16_t _destPort,
                                            const IP& destIp )
{
   IpConnection::fillNoConnectionHeader(
      pHdr, dataLength + sizeof( UdpHeader ) - sizeof( IpHeader ),
      IpHeader::UDP_PROTOCOL, destIp );

   pHdr->setSourcePort( getLastPort() );
   pHdr->setDestinationPort( _destPort );
   pHdr->setLength( sizeof( UdpHeader ) - sizeof( IpHeader ) + dataLength );
   pHdr->setChecksum();
}

void UdpConnection::convertHeaderToResponse( UdpHeader* pHdr, uint16_t dataLength )
{
   IpConnection::fillNoConnectionHeader(
      pHdr, dataLength + sizeof( UdpHeader ) - sizeof( IpHeader ),
      IpHeader::UDP_PROTOCOL, *pHdr->getSourceAddress() );

   uint16_t tmpPort = pHdr->getSourcePort();
   pHdr->setSourcePort( pHdr->getDestinationPort() );
   pHdr->setDestinationPort( tmpPort );
   pHdr->setLength( sizeof( UdpHeader ) - sizeof( IpHeader ) + dataLength );
   pHdr->setChecksum();
}

IStream::Status UdpConnection::genericCommand( IoStream::Command command, void* buffer )
{
   if ( command == IoStream::INIT )
   {
      IoStream::CommandINIT* cmd = (IoStream::CommandINIT*) buffer;
      setItsUser( cmd->owner );
      return SUCCESS;
   }
   return NOT_SUPPORTED;
}

IStream::Status UdpConnection::read( void* pData, uint16_t length, EventDrivenUnit* user )
{
   return NOT_SUPPORTED;
}

IStream::Status UdpConnection::write( void* pData, uint16_t length, EventDrivenUnit* user )
{
   if ( !stream )
   {
      return INVALID_STREAM;
   }

   uint8_t buffer[sizeof( UdpHeader ) + length];
   memcpy( &buffer[sizeof( UdpHeader )], pData, length );
   fillNoConnectionHeader( (UdpHeader*)buffer, length, remotePort, remoteIp );

   // ArpManager might change the packet into an arp-request, in this case the return value will be false
   bool success = ArpManager::prepareOutPacket( (IpHeader*) buffer, length );
   if ( ( stream->write( buffer, length ) != length ) || !success )
   {
      return ABORTED;
   }
   return SUCCESS;
}

