/*
 * HBCP.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "HBCP.h"
#include <CriticalSection.h>
#include <Security/Checksum.h>


const uint8_t HBCP::debugLevel( DEBUG_LEVEL_OFF );

uint16_t HBCP::deviceId;

uint16_t HBCP::receiverIdForErrors( 0 );

uint16_t HBCP::messagesInUse( 0 );



uint16_t HBCP::ControlFrame::getLength() const
{
   return ( sizeof( *this ) - sizeof( data ) + dataLength );
}

void HBCP::operator delete( void* obj, size_t size )
{
   CriticalSection cs;

   HBCP* me = ( (HBCP*) obj );

   if ( me->header.getReferenceCount() )
   {
      if ( me->header.decrementReferenceCount() == 0 )
      {
         DEBUG_M4( FSTR( "del " ), (uintptr_t ) me, FSTR( " ref " ), me->header.getReferenceCount() );
         delete (uint8_t*) obj;
         if ( messagesInUse )
         {
            messagesInUse--;
         }
         DEBUG_L2( FSTR( " use " ), messagesInUse );
      }
   }
}

HBCP* HBCP::copy()
{
   CriticalSection doNotInterrupt;
   HBCP* newMsg = (HBCP*) new uint8_t[( getLength() + 7 ) & 0xFFF8];
   if ( newMsg )
   {
      memcpy( (uint8_t*) newMsg, this, getLength() );
      newMsg->header.initReferenceCount();
      messagesInUse++;
      DEBUG_M4( FSTR( "new " ), (uintptr_t ) newMsg, FSTR( " ref " ), newMsg->header.getReferenceCount() );
      DEBUG_L2( FSTR( " use " ), messagesInUse );
   }
   return newMsg;
}


void HBCP::ControlFrame::encrypt()
{
   control = 0;
   control = Checksum::get( this, getLength() );
}


bool HBCP::ControlFrame::isValid()
{
   return !Checksum::hasError( this, getLength() );
}