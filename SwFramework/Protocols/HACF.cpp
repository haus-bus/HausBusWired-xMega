/********************************************************************
   Rhapsody	: 8.0.3
   Login		: viktor.pankraz
   Component	: AR8
   Configuration        : release
   Model Element	: HACF
   //!	Generated Date	: Wed, 6, Aug 2014
   File Path	: AR8/release/SwFramework/Protocols/HomeAutomationControlFrame/HACF.cpp
 *********************************************************************/

// ## auto_generated
#include "HACF.h"
// ## dependency CriticalSection
#include <CriticalSection.h>
#include <Security/Checksum.h>

// ## package SwFramework::Protocols::HomeAutomationControlFrame

// ## class HAC
const uint8_t HACF::debugLevel( DEBUG_LEVEL_OFF );

uint16_t HACF::deviceId;

uint16_t HACF::receiverIdForErrors( 0 );

uint16_t HACF::messagesInUse( 0 );

// ## class HACF::Object

// ## class HACF::Header

// ## class HACF::ControlFrame
uint16_t HACF::ControlFrame::getLength() const
{
   // #[ operation getLength() const
   return ( sizeof( *this ) - sizeof( data ) + dataLength );
   // #]
}

void HACF::operator delete( void* obj, size_t size )
{
   CriticalSection cs;

   HACF* me = ( (HACF*) obj );

   if ( me->header.getReferenceCount() )
   {
      if ( me->header.decrementReferenceCount() == 0 )
      {
         delete (uint8_t*) obj;
         if ( messagesInUse )
         {
            messagesInUse--;
         }
         DEBUG_M4( FSTR( "del " ), (uintptr_t ) me, FSTR( " ref " ), me->header.getReferenceCount() );
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


/*********************************************************************
   File Path	: AR8/release/SwFramework/Protocols/HomeAutomationControlFrame/HACF.cpp
*********************************************************************/
