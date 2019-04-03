/*
 * HmWStream.cpp
 *
 * Created: 18.11.2017 17:32:07
 * Author: viktor.pankraz
 */


#include "HmwStream.h"

HmwStream::MessageQueue HmwStream::inMessageQueue;

HmwStream::MessageVector HmwStream::outMessageVector;

IStream::Status HmwStream::sendMessage( HmwMessageBase& msg )
{
   IStream::Status status = HmwStreamBase::sendMessage( msg );
   // make sure the message is back in LittleEndian format after sending
   msg.convertToLittleEndian();
   msg.notifySending();

   // for INFO messages that are not sent broadcast we expect an ACK
   // if ACK is not received, message should be sent again after approx. 100ms
   if ( msg.isInfo() && !msg.isBroadcast() )
   {
      if ( msg.getSendingTries() == 1 )
      {
         // it was the first try to send, put message into outVector to check for ACK later or to resend
         if ( !outMessageVector.isFull() )
         {
            HmwMessageBase* outMsg = msg.copy();
            if ( outMsg )
            {
               outMessageVector.add( outMsg );
            }
            else
            {
               ERROR_1( FSTR( "HmwStream::sendMessage() out of memory" ) );
            }
         }
         else
         {
            ERROR_1( FSTR( "HmwStream::sendMessage() outMessageVector.isFull()" ) );
         }
      }
   }
   return status;
}
