/*
 * HACF.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "HACF.h"
#include <CriticalSection.h>
#include <Security/Checksum.h>


const uint8_t HACF::debugLevel( DEBUG_LEVEL_OFF );

uint16_t HACF::deviceId;

uint16_t HACF::receiverIdForErrors( 0 );

uint16_t HACF::messagesInUse( 0 );



uint16_t HACF::ControlFrame::getLength() const
{
   return ( sizeof( *this ) - sizeof( data ) + dataLength );
}

void HACF::operator delete( void* obj, size_t size )
{
   CriticalSection cs;

   HACF* me = ( (HACF*) obj );

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

HACF* HACF::copy()
{
   CriticalSection doNotInterrupt;
   HACF* newMsg = (HACF*) new uint8_t[( getLength() + 7 ) & 0xFFF8];
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


void HACF::ControlFrame::encrypt()
{
   control = 0;
   control = Checksum::get( this, getLength() );
}


bool HACF::ControlFrame::isValid()
{
   return !Checksum::hasError( this, getLength() );
}