/*
 * UdpConnection.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_IpStack_UdpConnection_H
#define SwFramework_Protocols_IpStack_UdpConnection_H

#include "IpStack.h"
#include "IpConnection.h"
#include <IoStream.h>

class UdpHeader;

class UdpConnection : public IpConnection,
                      public IoStream
{
   public:

      static const uint8_t MAX_UDP_CONNECTIONS = 4;

      ////    Constructors and destructors    ////

      inline UdpConnection()
      {
      }

      ////    Operations    ////

      static UdpConnection* connect( const IP& _remoteIp, uint16_t _remotePort, uint16_t _localPort, EventDrivenUnit* _user );

      static UdpConnection* getConnection( UdpHeader* packet );

      static void fillNoConnectionHeader( UdpHeader* pHdr, uint16_t dataLength, uint16_t _destPort, const IP& destIp = IP::broadcast() );

      static void convertHeaderToResponse( UdpHeader* pHdr, uint16_t dataLength );

      virtual IStream::Status genericCommand( Command command, void* buffer );

      virtual IStream::Status read( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

      virtual IStream::Status write( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

      ////    Additional operations    ////

   private:

      static UdpConnection connections[MAX_UDP_CONNECTIONS];

      ////    Attributes    ////

};

#endif
