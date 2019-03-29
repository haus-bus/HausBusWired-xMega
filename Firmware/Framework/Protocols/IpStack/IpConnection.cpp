/*
 * IpConnection.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "IpConnection.h"
#include "Security/Checksum16.h"
#include "Protocols/Ethernet/IpHeader.h"

const uint8_t debugLevel( DEBUG_LEVEL_OFF );

IpConnection::ListeningPorts IpConnection::listenports[MAX_LISTENPORTS];

uint16_t IpConnection::lastPort( FIRST_FREE_PORT );

uint16_t IpConnection::idCounter( 0 );

Enc28j60* IpConnection::stream( NULL );

void IpConnection::fillNoConnectionHeader( IpHeader* pHdr, uint16_t dataLength,
                                           uint8_t protocol,
                                           const IP& _remoteIp,
                                           uint8_t _timeToLive )
{
   // IP HEADER
   pHdr->destinationAddress = _remoteIp;
   pHdr->sourceAddress = IP::local;
   pHdr->setPacketSize( sizeof( IpHeader ) - sizeof( EthernetHeader ) + dataLength );
   pHdr->setProtocol( protocol );
   pHdr->setFragmentOffset( IpHeader::DO_NOT_FRAGMENT );
   pHdr->setTimeToLive( _timeToLive );
   pHdr->setId( ++idCounter );
   pHdr->setVersion( 4 );
   pHdr->setLength( ( sizeof( IpHeader ) - sizeof( EthernetHeader ) ) >> 2 );
   pHdr->setTypeOfService( 0 );
   pHdr->setChecksum();
}

void IpConnection::incLastPort()
{
   lastPort++;
   if ( lastPort >= LAST_FREE_PORT )
   {
      lastPort = FIRST_FREE_PORT;
   }
}

EventDrivenUnit* IpConnection::isListeningToPort( uint16_t port )
{
   uint8_t i = MAX_LISTENPORTS;
   while ( i-- )
   {
      if ( listenports[i].number == port )
      {
         return listenports[i].user;
      }
   }
   return 0;
}

bool IpConnection::listen( uint16_t port, EventDrivenUnit* user )
{
   uint8_t i = MAX_LISTENPORTS;
   while ( i-- )
   {
      if ( listenports[i].user == 0 )
      {
         listenports[i].user = user;
         listenports[i].number = port;
         return true;
      }
   }
   return false;
}

void IpConnection::unlisten( uint16_t port )
{
   uint8_t i = MAX_LISTENPORTS;
   while ( i-- )
   {
      if ( listenports[i].number == port )
      {
         listenports[i].user = 0;
         listenports[i].number = 0;
      }
   }
}

EventDrivenUnit* IpConnection::getItsUser() const
{
   return itsUser;
}

void IpConnection::setItsUser( EventDrivenUnit* p_EventDrivenUnit )
{
   itsUser = p_EventDrivenUnit;
}
