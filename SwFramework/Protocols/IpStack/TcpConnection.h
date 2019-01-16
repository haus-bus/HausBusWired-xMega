/*
 * TcpConnection.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_IpStack_TcpConnection_H
#define SwFramework_Protocols_IpStack_TcpConnection_H

#include "IpStack.h"
#include "IpConnection.h"

class Checksum16;

class Event;

class EventDrivenUnit;

class FlashString;

class IP;

class IStream;

class TcpHeader;

class TcpConnection : public IpConnection
{
   public:

      enum States
      {
         CLOSED,
         SYN_RCVD,
         SYN_SENT,
         ESTABLISHED,
         FIN_WAIT_1,
         FIN_WAIT_2,
         CLOSING,
         TIME_WAIT,
         LAST_ACK,
         STATE_MASK = 0x0F
      };

#define MAX_TCP_CONNECTIONS     4
#define RETRANSMISSION_TIMEOUT  3
#define MAX_SEGMENT_SIZE        ( IP_STACK_BUFFER_SIZE - 64 )

      ////    Constructors and destructors    ////

      inline TcpConnection() :
         mainState( CLOSED ), stopped( false )
      {

      }

      ////    Operations    ////

      static TcpConnection* connect( const IP& _remoteIp, uint16_t _remotePort,
                                     EventDrivenUnit* _user );

      static TcpConnection* connect( const TcpHeader* tcp, EventDrivenUnit* _user );

      void fillHeader( TcpHeader* pHdr, uint16_t dataLength = 0 );

      static void fillNoConnectionHeader( TcpHeader* pHdr,
                                          uint16_t dataLength = 0 );

      inline static TcpConnection* getConnection( uint8_t idx );

      static TcpConnection* getConnection( TcpHeader* packet );

      static FlashString* getId();

      static TcpConnection* getUnusedConnection();

      inline bool inCLOSED();

      inline bool inESTABLISHED();

      inline bool inLAST_ACK();

      inline bool inSYN_RCVD();

      inline bool inSYN_SENT();

      bool isExpectedACK( const TcpHeader* packet );

      void notifyReceivedPacket( TcpHeader* packet );

      void parseTcpOptions( const TcpHeader* tcp );

      void reset();

      void setMainState( const States& p_mainState );

      ////    Additional operations    ////

      inline States getMainState() const
      {
         return mainState;
      }

      inline bool getStopped() const
      {
         return stopped;
      }

      inline void setStopped( bool p_stopped )
      {
         stopped = p_stopped;
      }

      inline uint16_t getInitialMaxSegmentSize() const
      {
         return initialMaxSegmentSize;
      }

      inline void setInitialMaxSegmentSize( uint16_t p_initialMaxSegmentSize )
      {
         initialMaxSegmentSize = p_initialMaxSegmentSize;
      }

      inline static uint32_t getInitialSeqNumber()
      {
         return initialSeqNumber;
      }

      inline static void setInitialSeqNumber( uint32_t p_initialSeqNumber )
      {
         initialSeqNumber = p_initialSeqNumber;
      }

      inline uint16_t getLastLength() const
      {
         return lastLength;
      }

      inline void setLastLength( uint16_t p_lastLength )
      {
         lastLength = p_lastLength;
      }

      inline uint16_t getMaxSegmentSize() const
      {
         return maxSegmentSize;
      }

      inline void setMaxSegmentSize( uint16_t p_maxSegmentSize )
      {
         maxSegmentSize = p_maxSegmentSize;
      }

      inline uint32_t getReceiveNextSequenceNumber() const
      {
         return receiveNextSequenceNumber;
      }

      inline void setReceiveNextSequenceNumber(
         uint32_t p_receiveNextSequenceNumber )
      {
         receiveNextSequenceNumber = p_receiveNextSequenceNumber;
      }

      inline uint8_t getRetransmissionCounter() const
      {
         return retransmissionCounter;
      }

      inline void setRetransmissionCounter( uint8_t p_retransmissionCounter )
      {
         retransmissionCounter = p_retransmissionCounter;
      }

      inline uint8_t getRetransmissionTimeout() const
      {
         return retransmissionTimeout;
      }

      inline void setRetransmissionTimeout( uint8_t p_retransmissionTimeout )
      {
         retransmissionTimeout = p_retransmissionTimeout;
      }

      inline uint8_t getRetransmissionTimer() const
      {
         return retransmissionTimer;
      }

      inline void setRetransmissionTimer( uint8_t p_retransmissionTimer )
      {
         retransmissionTimer = p_retransmissionTimer;
      }

      inline uint8_t getSa() const
      {
         return sa;
      }

      inline void setSa( uint8_t p_sa )
      {
         sa = p_sa;
      }

      inline uint32_t getSendNextSequenceNumber() const
      {
         return sendNextSequenceNumber;
      }

      inline void setSendNextSequenceNumber( uint32_t p_sendNextSequenceNumber )
      {
         sendNextSequenceNumber = p_sendNextSequenceNumber;
      }

      inline uint8_t getSv() const
      {
         return sv;
      }

      inline void setSv( uint8_t p_sv )
      {
         sv = p_sv;
      }

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

   private:

      ////    Attributes    ////

   public:

      static uint32_t initialSeqNumber;

      States mainState : 7;

      bool stopped : 1;

      uint16_t initialMaxSegmentSize;

      uint16_t lastLength;

      uint16_t maxSegmentSize;

      uint32_t receiveNextSequenceNumber;

      uint8_t retransmissionCounter;

      uint8_t retransmissionTimeout;

      uint8_t retransmissionTimer;

      uint8_t sa;

      uint32_t sendNextSequenceNumber;

      uint8_t sv;

   private:

      static TcpConnection connections[MAX_TCP_CONNECTIONS];

   protected:

      static const uint8_t debugLevel;
};

inline TcpConnection* TcpConnection::getConnection( uint8_t idx )
{
   if ( idx < MAX_TCP_CONNECTIONS )
   {
      return &connections[idx];
   }
   return 0;
}

inline bool TcpConnection::inCLOSED()
{
   return ( mainState == CLOSED );
}

inline bool TcpConnection::inESTABLISHED()
{
   return ( mainState == ESTABLISHED );
}

inline bool TcpConnection::inLAST_ACK()
{
   return ( mainState == LAST_ACK );
}

inline bool TcpConnection::inSYN_RCVD()
{
   return ( mainState == SYN_RCVD );
}

inline bool TcpConnection::inSYN_SENT()
{
   return ( mainState == SYN_SENT );
}

#endif
