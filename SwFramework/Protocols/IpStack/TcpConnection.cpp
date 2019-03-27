/*
 * TcpConnection.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "TcpConnection.h"
#include "Security/Checksum16.h"
#include "EventPkg/EventPkg.h"
#include "EventDrivenUnit.h"
#include <FlashString.h>
#include "Protocols/Ethernet/IP.h"
#include "IStream.h"
#include "Protocols/Ethernet/TcpHeader.h"

uint32_t TcpConnection::initialSeqNumber( 1234 );

TcpConnection TcpConnection::connections[MAX_TCP_CONNECTIONS];

const uint8_t TcpConnection::debugLevel( DEBUG_LEVEL_OFF | DEBUG_STATE_MASK );

FlashString* TcpConnection::getId()
{
   return FSTR( "TcpConnection" );
}

TcpConnection* TcpConnection::connect( const IP& _remoteIp,
                                       uint16_t _remotePort,
                                       EventDrivenUnit* _user )
{
   uint8_t c = MAX_TCP_CONNECTIONS;
   while ( c-- )
   {
      if ( connections[c].isLastPortUsed() )
      {
         incLastPort();
         c = MAX_TCP_CONNECTIONS;
      }
   }

   TcpConnection* connection = getUnusedConnection();
   if ( connection )
   {
      connection->SET_STATE_L1( SYN_SENT )
      ;
      connection->setSendNextSequenceNumber( initialSeqNumber );
      connection->setInitialMaxSegmentSize( MAX_SEGMENT_SIZE );
      connection->setMaxSegmentSize( MAX_SEGMENT_SIZE );

      connection->setLastLength( 1 ); // TCP length of the SYN is one.
      connection->setRetransmissionCounter( 0 );
      connection->setRetransmissionTimer( 1 );
      connection->setRetransmissionTimeout( RETRANSMISSION_TIMEOUT );
      connection->sa = 0;
      connection->sv = 16; // Initial value of the RTT variance.
      connection->localPort = lastPort;
      connection->remotePort = _remotePort;
      connection->remoteIp = _remoteIp;
   }
   return connection;
}

TcpConnection* TcpConnection::connect( const TcpHeader* tcp,
                                       EventDrivenUnit* _user )
{
   TcpConnection* connection = getUnusedConnection();
   if ( connection )
   {
      connection->setItsUser( _user );
      connection->SET_STATE_L1( SYN_RCVD )
      ;
      connection->setSendNextSequenceNumber( initialSeqNumber );
      connection->setInitialMaxSegmentSize( MAX_SEGMENT_SIZE );
      connection->setMaxSegmentSize( MAX_SEGMENT_SIZE );

      connection->setLastLength( 1 ); // TCP length of the SYN is one.
      connection->setRetransmissionCounter( 0 );
      connection->setRetransmissionTimer( RETRANSMISSION_TIMEOUT );
      connection->setRetransmissionTimeout( RETRANSMISSION_TIMEOUT );
      connection->sa = 0;
      connection->sv = 4;
      connection->localPort = tcp->getDestinationPort();
      connection->remotePort = tcp->getSourcePort();
      connection->remoteIp = tcp->sourceAddress;
      connection->setReceiveNextSequenceNumber( tcp->getSeqNumber() + 1 );
      connection->parseTcpOptions( tcp );

   }
   return connection;
}

void TcpConnection::fillHeader( TcpHeader* pHdr, uint16_t dataLength )
{
   IpConnection::fillHeader( pHdr,
                             dataLength + sizeof( TcpHeader ) - sizeof( IpHeader ),
                             IpHeader::TCP_PROTOCOL );

   // TCP HEADER
   pHdr->setAckNumber( receiveNextSequenceNumber );
   pHdr->setSeqNumber( sendNextSequenceNumber );

   // If the connection has issued stop(), we advertise a zero
   // window so that the remote host will stop sending data.
   pHdr->setWindow( stopped ? 0 : MAX_SEGMENT_SIZE );
   pHdr->setSourcePort( localPort );
   pHdr->setDestinationPort( remotePort );
   pHdr->setUrgentPointer( 0 );
   pHdr->setChecksum();
}

void TcpConnection::fillNoConnectionHeader( TcpHeader* pHdr,
                                            uint16_t dataLength )
{
   IpConnection::fillNoConnectionHeader(
      pHdr, dataLength + sizeof( TcpHeader ) - sizeof( IpHeader ),
      IpHeader::TCP_PROTOCOL );

   // TCP HEADER
   pHdr->setUrgentPointer( 0 );
   pHdr->setChecksum();
}

TcpConnection* TcpConnection::getConnection( TcpHeader* packet )
{
   uint8_t c = MAX_TCP_CONNECTIONS;
   while ( c-- )
   {
      TcpConnection* con = &connections[c];
      if ( ( con->getItsUser() != 0 ) && !con->inCLOSED() )
      {
         if ( con->localPort == packet->getDestinationPort() )
         {
            if ( con->remotePort == packet->getSourcePort() )
            {
               if ( con->remoteIp == packet->sourceAddress )
               {
                  return con;
               }
            }
         }
      }
   }
   return 0;
}

TcpConnection* TcpConnection::getUnusedConnection()
{
   TcpConnection* connection = 0;
   uint8_t c = MAX_TCP_CONNECTIONS;
   while ( c-- )
   {
      TcpConnection& con = connections[c];
      if ( !con.itsUser || ( con.mainState == CLOSED ) )
      {
         connection = &connections[c];
         break;
      }
      else
      {
         if ( con.mainState == TIME_WAIT )
         {
            if ( !connection
               || ( con.retransmissionTimer > connection->retransmissionTimer ) )
            {
               connection = &con;
            }
         }
      }
   }
   return connection;
}

bool TcpConnection::isExpectedACK( const TcpHeader* packet )
{
   if ( packet->isACK() && lastLength )
   {
      uint32_t expectedAckNumber = sendNextSequenceNumber + lastLength;
      if ( expectedAckNumber == packet->getAckNumber() )
      {
         sendNextSequenceNumber = expectedAckNumber;
         // Do RTT estimation, unless we have done retransmissions.
         if ( retransmissionCounter == 0 )
         {
            signed char m = retransmissionTimeout - retransmissionTimer;
            m = m - ( sa >> 3 );
            sa += m;
            if ( m < 0 )
            {
               m = -m;
            }
            m = m - ( sv >> 2 );
            sv += m;
            retransmissionTimeout = ( sa >> 3 ) + sv;

         }
         // Reset the retransmission timer.
         retransmissionTimer = retransmissionTimeout;

         // Reset length of outstanding data.
         lastLength = 0;
         return true;
      }
   }
   return false;
}

void TcpConnection::notifyReceivedPacket( TcpHeader* packet )
{
   // Next, check if the incoming segment acknowledges any outstanding
   // data. If so, we update the sequence number, reset the length of
   // the outstanding data, calculate RTT estimations, and reset the
   // retransmission timer.
   ackData = isExpectedACK( packet );
   uint16_t dataLength = packet->getDataLength();

   DEBUG_H4( FSTR( "packet received: " ), dataLength, ' ', (uint8_t )ackData );

   // Do different things depending on in what state the connection is
   if ( inSYN_RCVD() )
   {
      // In SYN_RCVD we have sent out a SYNACK in response to a SYN, and
      // we are waiting for an ACK that acknowledges the data we sent
      // out the last time. Therefore, we want to have the ackData
      // flag set. If so, we enter the ESTABLISHED state.
      if ( ackData )
      {
         SET_STATE_L1( ESTABLISHED );
         lastLength = 0;
         receiveNextSequenceNumber += dataLength;
         packet->setPacketSize( 0 );
         return;
      }
      DEBUG_M1( FSTR( "reset in SYN_RCVD" ) );
      packet->changeToResetAckPacket();
      fillHeader( packet );
   }
   else if ( inSYN_SENT() )
   {
      if ( ackData && packet->isSYN_ACK() )
      {
         SET_STATE_L1( ESTABLISHED );
         setReceiveNextSequenceNumber( packet->getSeqNumber() + 1 );
         parseTcpOptions( packet );
         packet->changeToAckPacket();
      }
      else
      {
         SET_STATE_L1( CLOSED );
         packet->changeToResetPacket();
      }
      fillHeader( packet );
   }
   else if ( inESTABLISHED() )
   {
      // In the ESTABLISHED state, we call upon the application to feed
      // data into the uip_buf. If the UIP_ACKDATA flag is set, the
      // application should put new data into the buffer, otherwise we are
      // retransmitting an old segment, and the application should put that
      // data into the buffer.

      // If the incoming packet is a FIN, we should close the connection on
      // this side as well, and we send out a FIN and enter the LAST_ACK
      // state. We require that there is no outstanding data; otherwise the
      // sequence numbers will be screwed up.
      if ( packet->isFIN() && !stopped )
      {
         DEBUG_M1( FSTR( "FIN" ) );
         if ( lastLength )
         {
            ERROR_1( FSTR( "lastLength != 0" ) );
            packet->setPacketSize( 0 );
            return;
         }
         receiveNextSequenceNumber += ( dataLength + 1 );
         lastLength = 1;
         SET_STATE_L1( LAST_ACK );
         retransmissionCounter = 0;
         packet->changeToFinAckPacket();
         fillHeader( packet );
         return;
      }

      // If dataLength > 0 we have TCP data in the packet, and we flag this
      // by setting the UIP_NEWDATA flag and update the sequence number
      // we acknowledge. If the application has stopped the dataflow,
      // we must not accept any data packets from the remote host.
      if ( dataLength > 0 && !stopped )
      {
         DEBUG_H1( FSTR( "new data" ) );
         newData = true;
         receiveNextSequenceNumber += dataLength;
      }
      // Check if the available buffer space advertised by the other end
      // is smaller than the initial MSS for this connection. If so, we
      // set the current MSS to the window size to ensure that the
      // application does not send more data than the other end can handle.

      // If the remote host advertises a zero window, we set the MSS to
      // the initial MSS so that the application will send an entire MSS
      // of data. This data will not be acknowledged by the receiver,
      // and the application will retransmit it. This is called the
      // "persistent timer" and uses the retransmission mechanim.

      uint16_t window = packet->getWindow();
      if ( !window || ( window > initialMaxSegmentSize ) )
      {
         window = initialMaxSegmentSize;
      }
      maxSegmentSize = window;

      IStream::TransferDescriptor td;
      td.pData = packet->getData();
      if ( newData )
      {
         td.bytesTransferred = packet->getDataLength();
         td.bytesRemaining = 0;
         abort = !itsUser->notifyEvent( evData( itsUser, &td ) );
      }
      else if ( ackData )
      {
         td.bytesTransferred = lastLength;
         td.bytesRemaining = maxSegmentSize;
         close = !itsUser->notifyEvent(
            evEndOfTransfer( itsUser, &td, IStream::SUCCESS ) );
      }

      if ( abort || close )
      {
         if ( abort )
         {
            SET_STATE_L1( CLOSED );
            packet->changeToResetAckPacket();
         }
         if ( close )
         {
            lastLength = 1;
            SET_STATE_L1( FIN_WAIT_1 );
            retransmissionCounter = 0;
            packet->changeToFinAckPacket();
         }
         fillHeader( packet );
      }
      else
      {
         if ( newData || td.bytesTransferred )
         {
            if ( newData )
            {
               packet->changeToAckPacket();
            }
            lastLength = td.bytesTransferred;
            fillHeader( packet, td.bytesTransferred );
         }
         else
         {
            packet->setPacketSize( 0 );
         }
      }
      newData = false;
   }
   else if ( inLAST_ACK() )
   {
      if ( ackData )
      {
         SET_STATE_L1( CLOSED );
      }
      packet->setPacketSize( 0 );
   }
   else
   {
      // TODO
      ERROR_1( FSTR( "this state is not implemented now " ) );
   }
   /* CLOSED and LISTEN are not handled here. CLOSE_WAIT is not
      implemented, since we force the application to close when the
      peer sends a FIN (hence the application goes directly from
      ESTABLISHED to LAST_ACK). */

}

void TcpConnection::parseTcpOptions( const TcpHeader* tcp )
{
   // parse the TCP MSS option, if present
   const uint8_t* opt = tcp->getOptions();
   for ( uint8_t i = 0; i < tcp->getDataOffset(); )
   {
      if ( opt[i] == tcp->OPT_END )
      {
         // End of options
         break;
      }
      else if ( opt[i] == tcp->OPT_NOOP )
      {
         i++; // no option, skip
      }
      else if ( ( opt[i] == tcp->OPT_MSS ) && ( opt[i + 1] == tcp->OPT_MSS_LEN ) )
      {
         // An MSS option with the right option length
         uint16_t mss = ( (uint16_t) opt[i + 2] << 8 ) | opt[i + 3];
         if ( mss > MAX_SEGMENT_SIZE )
         {
            mss = MAX_SEGMENT_SIZE;
         }
         setInitialMaxSegmentSize( mss );
         setMaxSegmentSize( mss );
         // this is the only option we support, so we are done
         break;
      }
      else
      {
         // All other options have a length field, so that we easily
         // can skip past them
         if ( opt[i + 1] == 0 )
         {
            // If the length field is zero, the options are malformed
            // and we don't process them further.
            break;
         }
         i += opt[i + 1];
      }
   }
}

void TcpConnection::reset()
{
   SET_STATE_L1( CLOSED );
   // TODO
   // notifyUser about closed connection
}

void TcpConnection::setMainState( const TcpConnection::States& p_mainState )
{
   mainState = p_mainState;
   if ( inCLOSED() || inESTABLISHED() )
   {
      itsUser->notifyEvent( evConnect( itsUser, inESTABLISHED() ) );
   }
}
