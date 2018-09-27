/*
 * IpConnection.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_IpStack_IpConnection_H
#define SwFramework_Protocols_IpStack_IpConnection_H

#include "IpStack.h"
#include "EventDrivenUnit.h"
#include "Protocols/Ethernet/IP.h"
#include <Enc28j60.h>

class Checksum16;

class IpHeader;

class IpConnection
{
   public:

      static const uint16_t FIRST_FREE_PORT = 1024;
      static const uint16_t LAST_FREE_PORT = 32000;
      static const uint8_t TIME_TO_LIVE = 64;
      static const uint8_t MAX_LISTENPORTS = 20;

      struct ListeningPorts
      {
         EventDrivenUnit* user;
         uint16_t number;
      };

      ////    Constructors and destructors    ////

      inline IpConnection() :
         ackData( false ), newData( false ), abort( false ), retransmit( false ),
         close( false ), connected( false ), timeouted( false ), timeToLive( 128 ),
         localPort( 0 ), remotePort( 0 )
      {
         itsUser = NULL;
      }

      ////    Operations    ////

      inline void fillHeader( IpHeader* pHdr, uint16_t dataLength,
                              uint8_t protocol );

      static void fillNoConnectionHeader( IpHeader* pHdr, uint16_t dataLength,
                                          uint8_t protocol, const IP& _remoteIp = IP::broadcast(),
                                          uint8_t _timeToLive = TIME_TO_LIVE );

      static void incLastPort();

      inline bool isLastPortUsed() const;

      static EventDrivenUnit* isListeningToPort( uint16_t port );

      static bool listen( uint16_t port, EventDrivenUnit* user );

      static void unlisten( uint16_t port );

      ////    Additional operations    ////

      inline static ListeningPorts getListenports( int i1 )
      {
         return listenports[i1];
      }

      inline static void setListenports( int i1, ListeningPorts p_listenports )
      {
         listenports[i1] = p_listenports;
      }

      inline static uint16_t getLastPort()
      {
         return lastPort;
      }

      inline static void setLastPort( uint16_t p_lastPort )
      {
         lastPort = p_lastPort;
      }

      inline static uint16_t getIdCounter()
      {
         return idCounter;
      }

      inline static void setIdCounter( uint16_t p_idCounter )
      {
         idCounter = p_idCounter;
      }

      inline bool getAckData() const
      {
         return ackData;
      }

      inline void setAckData( bool p_ackData )
      {
         ackData = p_ackData;
      }

      inline bool getNewData() const
      {
         return newData;
      }

      inline void setNewData( bool p_newData )
      {
         newData = p_newData;
      }

      inline bool getAbort() const
      {
         return abort;
      }

      inline void setAbort( bool p_abort )
      {
         abort = p_abort;
      }

      inline bool getRetransmit() const
      {
         return retransmit;
      }

      inline void setRetransmit( bool p_retransmit )
      {
         retransmit = p_retransmit;
      }

      inline bool getClose() const
      {
         return close;
      }

      inline void setClose( bool p_close )
      {
         close = p_close;
      }

      inline bool getConnected() const
      {
         return connected;
      }

      inline void setConnected( bool p_connected )
      {
         connected = p_connected;
      }

      inline bool getTimeouted() const
      {
         return timeouted;
      }

      inline void setTimeouted( bool p_timeouted )
      {
         timeouted = p_timeouted;
      }

      inline uint8_t getTimeToLive() const
      {
         return timeToLive;
      }

      inline void setTimeToLive( uint8_t p_timeToLive )
      {
         timeToLive = p_timeToLive;
      }

      inline uint16_t getLocalPort() const
      {
         return localPort;
      }

      inline void setLocalPort( uint16_t p_localPort )
      {
         localPort = p_localPort;
      }

      inline uint16_t getRemotePort() const
      {
         return remotePort;
      }

      inline void setRemotePort( uint16_t p_remotePort )
      {
         remotePort = p_remotePort;
      }

      inline IP* getRemoteIp() const
      {
         return (IP*) &remoteIp;
      }

      inline void setRemoteIp( const IP& _remoteIp )
      {
         remoteIp = _remoteIp;
      }

      inline bool isConnectionUsed() const
      {
         return ( itsUser != NULL ) || remoteIp.isValid();
      }

      EventDrivenUnit* getItsUser() const;

      void setItsUser( EventDrivenUnit* p_EventDrivenUnit );

      ////    Attributes    ////

      static Enc28j60* stream;

   protected:

      static ListeningPorts listenports[MAX_LISTENPORTS];

      static uint16_t lastPort;

      static uint16_t idCounter;

      bool ackData : 1;

      bool newData : 1;

      bool abort : 1;

      bool retransmit : 1;

      bool close : 1;

      bool connected : 1;

      bool timeouted : 1;

      uint8_t timeToLive;

      uint16_t localPort;

      uint16_t remotePort;

      IP remoteIp;

      ////    Relations and components    ////

   protected:

      EventDrivenUnit* itsUser;
};

inline void IpConnection::fillHeader( IpHeader* pHdr, uint16_t dataLength,
                                      uint8_t protocol )
{
   fillNoConnectionHeader( pHdr, dataLength, protocol, remoteIp, timeToLive );
}

inline bool IpConnection::isLastPortUsed() const
{
   return ( localPort == lastPort ) && ( itsUser != 0 );
}

#endif
