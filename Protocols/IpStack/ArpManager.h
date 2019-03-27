/*
 * ArpManager.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_IpStack_ArpManager_H
#define SwFramework_Protocols_IpStack_ArpManager_H

#include "IpStack.h"
#include "Protocols/Ethernet/IP.h"
#include "Protocols/Ethernet/MAC.h"

class ArpHeader;

class IpHeader;

class ArpManager
{
   public:

      struct TableEntry
      {
         IP ipAddress;
         MAC macAddress;
         uint8_t time;
      };

#define MAX_TABLE_ENTRIES 4
#define MAX_AGE 120

      ////    Operations    ////

      // ARP processing for incoming ARP packets.
      //
      // This function should be called by the device driver when an ARP
      // packet has been received. The function will act differently
      // depending on the ARP packet type: if it is a reply for a request
      // that we previously sent out, the ARP cache will be filled in with
      // the values from the ARP reply. If the incoming ARP packet is an ARP
      // request for our IP address, an ARP reply packet is created and put
      // into the buffer.
      //
      // When the function returns, the value of the global variable dataLength
      // indicates whether the device driver should send out a packet or
      // not. If dataLength is zero, no packet should be sent. If dataLength is
      // non-zero, it contains the length of the outbound packet that is
      // present in the buffer.
      static uint16_t notifyArpPacket( ArpHeader* arpHeader );

      // ARP processing for incoming IP packets
      //
      // This function should be called by the device driver when an IP
      // packet has been received. The function will check if the address is
      // in the ARP cache, and if so the ARP cache entry will be
      // refreshed. If no ARP cache entry was found, a new one is created.
      static void notifyIpPacket( const IpHeader* ipHeader );

      // Find the destination IP address in the ARP table and construct the Ethernet header.
      // If the destination IP addres isn't on the local network, we use the default router's IP address instead.
      //
      // If not ARP table entry is found, we overwrite the original IP packet with an ARP request for the IP address.
      static bool prepareOutPacket( IpHeader* ipHeader, uint16_t& len );

      // This function performs periodic timer processing in the ARP module
      // and should be called at regular intervals. The recommended interval
      // is 10 seconds between the calls.
      static void refreshTable();

      // Walk through the ARP mapping table and try to find an entry to update.
      // If none is found, the IP -> MAC address mapping is inserted in the ARP table.
      static void update( const IP& ip, const MAC& mac );

      ////    Additional operations    ////

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

   private:

      inline static TableEntry getTable( int i1 )
      {
         return table[i1];
      }

      inline static void setTable( int i1, TableEntry p_table )
      {
         table[i1] = p_table;
      }

      inline static uint8_t getTime()
      {
         return time;
      }

      inline static void setTime( uint8_t p_time )
      {
         time = p_time;
      }

      ////    Attributes    ////

   protected:

      static const uint8_t debugLevel;

   private:

      static TableEntry table[MAX_TABLE_ENTRIES];

      static uint8_t time;
};

#endif
