/*
 * IResponse.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "IResponse.h"

#include <EventPkg/EventPkg.h>
#include <Reactive.h>
#include <Scheduler.h>

IResponse::IResponse( uint16_t id )
{
   setup( id );
}

IResponse::IResponse( uint16_t id, const HACF& request )
{
   setup( id );
   setupForResult( request );
}

void IResponse::queue( Reactive* reactive )
{
   if ( !reactive )
   {
      reactive = Scheduler::getJob( HACF::SYSTEM_ID );
   }

   HACF* newMsg = copy();
   if ( newMsg )
   {
      if ( !evGatewayMessage( reactive, newMsg ).queue() )
      {
         ERROR_1( FSTR( "EventQueue is full" ) );
         delete newMsg;
      }
   }
}

void IResponse::setErrorCode( uint8_t errorCode, uint8_t* errorData )
{
   controlFrame.setDataLength( errorData ? ControlFrame::DEFAULT_DATA_LENGTH : 2 );
   uint8_t* pData = controlFrame.data;
   *pData++ = HACF::EVENT_ERROR;
   *pData++ = errorCode;
   if ( errorData )
   {
      for ( uint8_t i = 0; i < ( ControlFrame::DEFAULT_DATA_LENGTH - 2 ); i++ )
      {
         pData[i] = errorData[i];
      }
   }
}

void IResponse::setup( uint16_t id )
{
   header.setSenderId( id );
   controlFrame.receiverId.setId( HACF::BROADCAST_ID );
   controlFrame.senderId.setObjectId( id );
   controlFrame.senderId.setDeviceId( HACF::deviceId );
   controlFrame.setDataLength( 2 );
   setResponse( 0 );
}

void IResponse::setupForResult( const HACF& request )
{
   controlFrame.receiverId.setId( request.controlFrame.senderId.getId() );
   controlFrame.packetCounter = request.controlFrame.packetCounter;
}
