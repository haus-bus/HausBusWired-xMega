/*
 * BaseSensorUnit.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "BaseSensorUnit.h"

const uint8_t BaseSensorUnit::debugLevel( DEBUG_LEVEL_OFF );

BaseSensorUnit::Response::Parameter& BaseSensorUnit::Response::setConfiguration()
{
   controlFrame.setDataLength( sizeof( getResponse() ) + sizeof( getParameter().configuration ) );
   setResponse( CONFIGURATION );
   return getParameter();
}

void BaseSensorUnit::Response::setStatus( BaseSensorUnit::Status value )
{
   controlFrame.setDataLength( sizeof( getResponse() ) + sizeof( getParameter().status ) );
   setResponse( STATUS );
   getParameter().status = value;
}

void BaseSensorUnit::notifyNewValue( BaseSensorUnit::Status newStatus )
{
   if ( !configuration )
   {
      lastStatus = newStatus;
      return;
   }

   if ( configuration->reportTimeBase == 0 )
   {
      SET_STATE_L1( IDLE );
   }

   uint8_t nextEvent = currentEvent;

   int16_t newStatusValue = newStatus.getCompleteValue();
   int16_t lowerThreshold = configuration->lowerThreshold * 100 + configuration->lowerThresholdFraction;
   int16_t upperThreshold = configuration->upperThreshold * 100 + configuration->upperThresholdFraction;
   int16_t hysteresis = configuration->hysteresis * 10;

   // | old | new | Event
   // ---------------------
   // |  b  |  b  |
   // |  b  |  r  | evInRange
   // |  b  |  a  | evAbove
   // |  r  |  b  | evBelow
   // |  r  |  r  |
   // |  r  |  a  | evAbove
   // |  a  |  b  | evBelow
   // |  a  |  r  | evInRange
   // |  a  |  a  |
   //
   // b, value is < lower
   // r, value is >= lower && < upper
   // a, value is >= upper

   if ( ( hysteresis == 0 ) || ( currentEvent == 0 ) )
   {
      // this is the first value after reset or hysteresis is disabled
      if ( newStatusValue < lowerThreshold )
      {
         nextEvent = Response::EVENT_BELOW;
      }
      else if ( newStatusValue >= upperThreshold )
      {
         nextEvent = Response::EVENT_ABOVE;
      }
      else
      {
         nextEvent = Response::EVENT_IN_RANGE;
      }
   }
   else // hysteresis != 0
   {
      if ( newStatusValue < lowerThreshold )
      {
         if ( lastEvent != Response::EVENT_BELOW )
         {
            nextEvent = Response::EVENT_BELOW;
         }
         else if ( newStatusValue < ( lowerThreshold - hysteresis ) )
         {
            nextEvent = Response::EVENT_BELOW;
         }
      }
      else if ( newStatusValue >= upperThreshold )
      {
         if ( lastEvent != Response::EVENT_ABOVE )
         {
            nextEvent = Response::EVENT_ABOVE;
         }
         else if ( newStatusValue > ( upperThreshold + hysteresis ) )
         {
            nextEvent = Response::EVENT_ABOVE;
         }
      }
      else
      {
         if ( lastEvent != Response::EVENT_IN_RANGE )
         {
            nextEvent = Response::EVENT_IN_RANGE;
         }
         else if ( ( newStatusValue > ( lowerThreshold + hysteresis ) )
                 || ( newStatusValue < ( upperThreshold - hysteresis ) ) )
         {
            nextEvent = Response::EVENT_IN_RANGE;
         }
      }
   }

   if ( nextEvent != currentEvent )
   {
      if ( currentEvent != 0 )
      {
         Response event( getId() );
         event.setEvent( nextEvent );
         event.queue();
      }
      lastEvent = currentEvent;
      currentEvent = nextEvent;
   }
   newStatus.lastEvent = currentEvent;

   timeSinceReport++;
   if ( timeSinceReport > configuration->minReportTime )
   {
      if ( ( timeSinceReport > configuration->maxReportTime )
         || ( abs( lastStatus.getCompleteValue() - newStatusValue ) > hysteresis ) )
      {
         timeSinceReport = 0;
         Response event( getId() );
         event.setStatus( newStatus );
         event.queue();
         lastStatus = newStatus;
      }
   }
}

bool BaseSensorUnit::handleRequest( HACF* message )
{
   if ( !message->controlFrame.isCommand() )
   {
      return false;
   }
   HACF::ControlFrame& cf = message->controlFrame;
   Command* data = reinterpret_cast<Command*>( cf.getData() );

   if ( cf.isCommand( Command::SET_CONFIGURATION ) )
   {
      DEBUG_H1( FSTR( ".setConfiguration()" ) );
      configuration->set( data->parameter.setConfiguration );
      timeSinceReport = configuration->maxReportTime;
   }
   else
   {
      Response response( getId(), *message );
      if ( cf.isCommand( Command::GET_CONFIGURATION ) )
      {
         DEBUG_H1( FSTR( ".getConfiguration()" ) );
         configuration->get( response.setConfiguration().configuration );
      }
      else if ( cf.isCommand( Command::GET_STATUS ) )
      {
         DEBUG_H1( FSTR( ".getStatus()" ) );
         response.setStatus( lastStatus );
         if ( inIdle() )
         {
            SET_STATE_L1( RUNNING );
            setSleepTime( WAKE_UP );
         }
      }
      else
      {
         return false;
      }
      response.queue( getObject( message->header.getSenderId() ) );

   }

   return true;
}
