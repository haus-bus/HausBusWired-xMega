/*
 * ArpManager.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "ArpManager.h"
#include "Protocols/Ethernet/ArpHeader.h"
#include "Protocols/Ethernet/IpHeader.h"

const uint8_t ArpManager::debugLevel( DEBUG_LEVEL_OFF );

ArpManager::TableEntry ArpManager::table[MAX_TABLE_ENTRIES];

uint8_t ArpManager::time( 0 );

uint16_t ArpManager::notifyArpPacket( ArpHeader* arpHeader )
{

   switch ( arpHeader->getOpcode() )
   {
      case ArpHeader::REQUEST:

         // ARP request. If it asked for our address, we send out a reply.
         if ( arpHeader->destinationIpAddress.isForMe() )
         {
            DEBUG_M1( FSTR( "Arp REQUEST" ) );
            // First, we register the one who made the request in our ARP table,
            // since it is likely that we will do more communication with this host in the future.
            update( arpHeader->sourceIpAddress, arpHeader->sourceHwAddress );

            arpHeader->setOpcode( ArpHeader::REPLY );
            arpHeader->destinationHwAddress = arpHeader->sourceHwAddress;
            arpHeader->destinationIpAddress = arpHeader->sourceIpAddress;
            arpHeader->sourceHwAddress = MAC::local;
            arpHeader->sourceIpAddress = IP::local;
            arpHeader->setPacketType( EthernetHeader::ARP );
            arpHeader->packetDestination = arpHeader->destinationHwAddress;
            arpHeader->packetSource = MAC::local;
            return sizeof( ArpHeader );
         }
         break;

      case ArpHeader::REPLY:

         // We insert or update the ARP table if it was meant for us.
         if ( arpHeader->destinationIpAddress.isForMe() )
         {
            DEBUG_M1( FSTR( "Arp REPLY" ) );
            update( arpHeader->sourceIpAddress, arpHeader->sourceHwAddress );
         }
         break;
   }
   return 0;
}

void ArpManager::notifyIpPacket( const IpHeader* ipHeader )
{
   // Only insert/update an entry if the source IP address of the
   // incoming IP packet comes from a host on the local network.
   if ( ipHeader->destinationAddress.isForMe() )
   {
      update( ipHeader->sourceAddress, ipHeader->packetSource );
   }
}

bool ArpManager::prepareOutPacket( IpHeader* ipHeader, uint16_t& len )
{
   // First check if destination is a local broadcast.
   if ( ipHeader->destinationAddress.isBroadcast()
      || ipHeader->destinationAddress.isLocalBroadcast() )
   {
      ipHeader->packetDestination.setBroadcast();
   }
   else
   {
      // Check if the destination address is on the local network.
      IP destIp = ipHeader->destinationAddress;
      if ( !ipHeader->destinationAddress.isInLocalNetwork() )
      {
         destIp = IP::router;
      }

      uint8_t i;
      for ( i = 0; i < MAX_TABLE_ENTRIES; i++ )
      {
         TableEntry& te = table[i];
         if ( te.ipAddress == destIp )
         {
            break;
         }
      }

      if ( i >= MAX_TABLE_ENTRIES )
      {
         // The destination address was not in our ARP table, so we
         // overwrite the IP packet with an ARP request.
         DEBUG_M1( FSTR( "Arp request destination" ) );
         ArpHeader& request = *(ArpHeader*) ipHeader;
         request.packetDestination.setBroadcast();
         request.packetSource = MAC::local;
         request.sourceHwAddress = MAC::local;
         request.destinationHwAddress = MAC();
         request.destinationIpAddress = destIp;
         request.sourceIpAddress = IP::local;

         request.setOpcode( ArpHeader::REQUEST );
         request.setHwType( ArpHeader::HWTYPE_ETH );
         request.setProtocol( EthernetHeader::IP_DATAGRAMM );
         request.setHwLength( sizeof( MAC ) );
         request.setProtocolLength( 4 );
         request.setPacketType( EthernetHeader::ARP );
         len = sizeof( ArpHeader );
         return false;
      }

      // Build an ethernet header.
      ipHeader->packetDestination = table[i].macAddress;
   }
   ipHeader->packetSource = MAC::local;
   ipHeader->setPacketType( EthernetHeader::IP_DATAGRAMM );

   len = ipHeader->getPacketSize() + sizeof( EthernetHeader );
   return true;
}

void ArpManager::refreshTable()
{
   ++time;
   uint8_t i = MAX_TABLE_ENTRIES;
   while ( i-- )
   {
      TableEntry& te = table[i];
      if ( te.ipAddress.isValid() && ( ( time - te.time ) >= MAX_AGE ) )
      {
         te.ipAddress = IP();
      }
   }
}

void ArpManager::update( const IP& ip, const MAC& mac )
{
   uint8_t i = MAX_TABLE_ENTRIES;
   uint8_t iOldest = MAX_TABLE_ENTRIES;
   uint8_t iUnused = MAX_TABLE_ENTRIES;
   while ( i-- )
   {
      TableEntry& te = table[i];
      if ( te.ipAddress.isValid() )
      {
         if ( te.ipAddress == ip )
         {
            te.macAddress = mac;
            te.time = time;
            return;
         }
         if ( iOldest == MAX_TABLE_ENTRIES )
         {
            iOldest = i;
         }
         else
         {
            if ( ( time - table[iOldest].time ) < ( time - table[i].time ) )
            {
               iOldest = i;
            }
         }
      }
      else
      {
         iUnused = i;
      }
   }

   // If we get here, no existing ARP table entry was found, so we create one.
   // If no unused entry is found, we take the oldest entry and throw it away.
   i = ( iUnused != MAX_TABLE_ENTRIES ) ? iUnused : iOldest;
   table[i].ipAddress = ip;
   table[i].macAddress = mac;
   table[i].time = time;

   DEBUG_M2( FSTR( "ArpManager::update() -> new entry at idx: " ), i );
   DEBUG_M2( FSTR( "IP : " ), ip );
   DEBUG_M2( FSTR( "MAC: " ), mac );

}
