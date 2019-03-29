/*
 * Dhcp.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Dhcp.h"
#include "UdpConnection.h"
#include "IpStackManager.h"

#include <EventPkg/EventPkg.h>
#include <Protocols/Ethernet/MAC.h>
#include <Time/SystemTime.h>

IP Dhcp::offeredLocalIp;

IP Dhcp::offeredNetmask;

IP Dhcp::offeredRouterIp;

IP Dhcp::serverIp;

uint8_t Dhcp::debugLevel( DEBUG_LEVEL_OFF );

Dhcp::Dhcp() :
   leaseTime( 0 ), retries( 0 )
{
   setId( ( ClassId::DHCP << 8 ) | 1 );
   setRemoteAccess( false );
}

uint8_t* Dhcp::addMessageType( uint8_t* ptr, Dhcp::MsgType type )
{
   *ptr++ = OPTION_MSG_TYPE;
   *ptr++ = 1;
   *ptr++ = type;
   return ptr;
}

uint8_t* Dhcp::addRequestIpAddr( uint8_t* ptr )
{
   *ptr++ = OPTION_REQ_IPADDR;
   *ptr++ = 4;
   memcpy( ptr, &offeredLocalIp, 4 );
   return ptr + 4;
}

uint8_t* Dhcp::addRequestOptions( uint8_t* ptr )
{
   *ptr++ = OPTION_REQ_LIST;
   *ptr++ = 3;
   *ptr++ = OPTION_SUBNET_MASK;
   *ptr++ = OPTION_ROUTER;
   *ptr++ = OPTION_DNS_SERVER;
   return ptr;
}

void Dhcp::createMessage( Dhcp::Message* msg )
{
   memset( msg, 0, sizeof( Message ) - sizeof( msg->options ) );
   msg->op = OPCODE_REQUEST;
   msg->hType = DHCP_HTYPE_ETHERNET;
   msg->hLength = sizeof( MAC );
   msg->xId = xId;
   msg->flags = BOOTP_BROADCAST;
   msg->ciAddr = IP::local.getAddress();
   memcpy( msg->chAddr, &MAC::local, sizeof( MAC::local ) );
   memcpy( msg->options, &magicCookie, sizeof( magicCookie ) );
}

bool Dhcp::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      run();
   }
   else if ( event.isEvData() )
   {
      IStream::TransferDescriptor* td = event.isEvData()->getTransferDescriptor();

      if ( td->bytesTransferred != 0 )
      {
         // we received new data
         uint8_t msgType = parseMessage( *( (Message*) td->pData ),
                                         td->pData + td->bytesTransferred );

         if ( ( state.sub == DISCOVER_SENT ) && ( msgType == OFFER ) )
         {
            SET_STATE_L2( OFFER_RECEIVED );
         }
         else if ( ( state.sub == REQUEST_SENT ) && ( msgType == ACK ) )
         {
            SET_STATE_L2( CONFIG_RECEIVED );
         }
         else if ( ( state.sub == REFRESH_SENT ) && ( msgType == ACK ) )
         {
            SET_STATE_L2( REFRESH_SUCCEEDED );
         }
         else if ( msgType == NAK )
         {
            SET_STATE_L2( NAK_RECEIVED );
         }
         else
         {
            return false;
         }
         setSleepTime( WAKE_UP );
      }
   }
   else if ( event.isEvConnect() )
   {
      const evConnect* ev = reinterpret_cast<const evConnect*>( &event );
      if ( ev->getStatus() )
      {
         // new connected or linked up, try to get new IP
         SET_STATE_L2( INITIAL );
         setSleepTime( WAKE_UP );
      }
      else
      {
         // TODO
      }
      DEBUG_H1( ( ev->getStatus() ? FSTR( "connected" ) : FSTR( "disconnected" ) ) );
   }
   return false;
}

void* Dhcp::operator new( size_t size )
{
   return allocOnce( size );
}

uint8_t Dhcp::parseMessage( const Dhcp::Message& msg, uint8_t* end )
{
   if ( ( msg.op == OPCODE_REPLY ) && ( msg.xId == xId )
      && ( memcmp( &msg.chAddr, &MAC::local, sizeof( MAC ) ) == 0 ) )
   {
      offeredLocalIp.setAddress( msg.yiAddr );
      return parseOptions( msg, end );
   }
   return 0;
}

uint8_t Dhcp::parseOptions( const Dhcp::Message& msg, uint8_t* end )
{
   uint8_t type = 0;
   uint8_t const* pOptions = &msg.options[4];

   while ( pOptions < end )
   {
      switch ( *pOptions )
      {
         case OPTION_SUBNET_MASK:
            memcpy( &offeredNetmask, pOptions + 2, 4 );
            break;
         case OPTION_ROUTER:
            memcpy( &offeredRouterIp, pOptions + 2, 4 );
            break;
         case OPTION_DNS_SERVER:
            // memcpy(status.dnsaddr, pOptions + 2, 4);
            break;
         case OPTION_MSG_TYPE:
            type = *( pOptions + 2 );
            break;
         case OPTION_SERVER_ID:
            memcpy( &serverIp, pOptions + 2, 4 );
            break;
         case OPTION_LEASE_TIME:
            memcpy( &leaseTime, pOptions + 2, 4 );
            break;
         case OPTION_END:
            return type;
      }
      pOptions += pOptions[1] + 2;
   }
   return type;
}

void Dhcp::run()
{
   if ( inStartUp() )
   {
      if ( !UdpConnection::isListeningToPort( CLIENT_PORT ) )
      {
         UdpConnection::listen( CLIENT_PORT, this );
      }
      SET_STATE_L1( RUNNING );
      SET_STATE_L2( INITIAL );
      setSleepTime( SystemTime::S );
   }
   else
   {
      switch ( state.sub )
      {
         case INITIAL:
         {
            IP::local.clear();
            sendDiscover();
            SET_STATE_L2( DISCOVER_SENT );
            setSleepTime( 3 * SystemTime::S );
            return;
         }
         case OFFER_RECEIVED:
         {
            sendRequest();
            SET_STATE_L2( REQUEST_SENT );
            setSleepTime( 3 * SystemTime::S );
            return;
         }
         case CONFIG_RECEIVED:
         {
            // apply the received settings
            IP::local = offeredLocalIp;
            IP::router = offeredRouterIp;
            IP::netmask = offeredNetmask;
            dumpOfferedSettings();
            SET_STATE_L2( REFRESH_SUCCEEDED );
            return;
         }
         case REFRESH_SUCCEEDED:
         {
            setSleepTime( SystemTime::S / 2 );
            if ( --leaseTime == 0 )
            {
               sendRequest();
               SET_STATE_L2( REFRESH_SENT );
               setSleepTime( 3 * SystemTime::S );
            }
            return;
         }
         default:
         {
            if ( retries++ > 10 )
            {
               SET_STATE_L2( TIMEOUT );
               setSleepTime( NO_WAKE_UP );

               // DHCP client failed, apply default settings
               if ( !IP::local.isValid() )
               {
                  IP::local = IP::defaultIp;
                  IP::router = IP::defaultRouterIp;
                  IP::netmask = IP::defaultNetmask;
               }
            }
            else
            {
               SET_STATE_L2( INITIAL );
            }
         }
      }
   }
}

void Dhcp::sendDiscover()
{
   Message msg;
   createMessage( &msg );
   uint8_t* end = addMessageType( &msg.options[4], DISCOVER );
   end = addRequestOptions( end );
   end = addEnd( end );

   if ( IpStackManager::instance() )
   {
      IpStackManager::instance()->sendToUdp( SERVER_PORT, &msg, end - (uint8_t*)&msg );
   }
   else
   {
      ERROR_1( FSTR( "No IpStackmanager available to send DISCOVER!" ) );
   }
}

void Dhcp::sendRequest()
{
   Message msg;
   createMessage( &msg );
   uint8_t* end = addMessageType( &msg.options[4], REQUEST );
   end = addServerId( end );
   end = addRequestIpAddr( end );
   end = addEnd( end );

   if ( IpStackManager::instance() )
   {
      IpStackManager::instance()->sendToUdp( SERVER_PORT, &msg, end - (uint8_t*)&msg );
   }
   else
   {
      ERROR_1( FSTR( "No IpStackmanager available to send REQUEST!" ) );
   }
}

void Dhcp::dumpOfferedSettings()
{
   DEBUG_H1( FSTR( "offered settings" ) );
   uint8_t const* ptr = (uint8_t*) &offeredLocalIp;
   DEBUG_M4( FSTR( "local  IP : " ), ptr[0], '.', ptr[1] );
   DEBUG_L4( '.', ptr[2], '.', ptr[3] );
   ptr = (uint8_t*) &offeredRouterIp;
   DEBUG_M4( FSTR( "router IP : " ), ptr[0], '.', ptr[1] );
   DEBUG_L4( '.', ptr[2], '.', ptr[3] );
   ptr = (uint8_t*) &offeredNetmask;
   DEBUG_M4( FSTR( "Netmask   : " ), ptr[0], '.', ptr[1] );
   DEBUG_L4( '.', ptr[2], '.', ptr[3] );
}
