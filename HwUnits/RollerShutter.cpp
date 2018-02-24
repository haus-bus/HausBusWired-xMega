/*
 * RollerShutter.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "RollerShutter.h"

uint8_t RollerShutter::startingChannels( 0 );

const uint8_t RollerShutter::debugLevel( DEBUG_LEVEL_OFF | DEBUG_STATE_L3 );

RollerShutter::Response::Parameter& RollerShutter::Response::setConfiguration()
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().configuration ) );
   setResponse( Response::CONFIGURATION );
   return getParameter();
}

void RollerShutter::Response::setDirection( uint8_t direction )
{
   setResponse( Response::EVENT_START );
   getParameter().direction = direction;
}

void RollerShutter::Response::setPosition( uint8_t position )
{
   if ( position )
   {
      setResponse( Response::EVENT_CLOSED );
      getParameter().position = position;
   }
   else
   {
      setResponse( Response::EVENT_OPEN );
      controlFrame.setDataLength( sizeof( getResponse() ) );
   }

}

void RollerShutter::Response::setStatus( uint8_t position )
{
   setResponse( Response::STATUS );
   getParameter().position = position;
}

RollerShutter::RollerShutter( uint8_t _id, RollerShutterHw* _hardware ) :
   subState( NO_COMMAND ), position( 0 ), targetPosition( 0 ), pollTime( 100 ),
   toggleDirection( TO_CLOSE )
{
   hardware = NULL;
   configuration = NULL;
   setId( ( ClassId::ROLLER_SHUTTER << 8 ) | _id );
   hardware = _hardware;
}

void RollerShutter::cmdMoveToPosition( uint8_t target )
{
   setTargetPosition( target );
   if ( inRunning() )
   {
      return;
   }

   if ( targetPosition != position )
   {
      SET_STATE_L1( RUNNING );
      if ( targetPosition > position )
      {
         hardware->setDirectionToClose();
         setPollTime(
            ( SystemTime::S / HwConfiguration::RollerShutter::MAX_LEVEL )
            * configuration->getCloseTime() );
      }
      else
      {
         hardware->setDirectionToOpen();
         setPollTime(
            ( SystemTime::S / HwConfiguration::RollerShutter::MAX_LEVEL )
            * configuration->getOpenTime() );
      }
      SET_STATE_L2( START_MOTOR );
      startingChannels++;
      DEBUG_M2( FSTR( "startingUnits+: " ), startingChannels );
      setSleepTime(
         startingChannels
         * HwConfiguration::RollerShutter::DEFAULT_MOTOR_START_DELAY );
   }
   else
   {
      cmdStop();
   }
}

void RollerShutter::cmdStop()
{
   if ( getSubState() == START_MOTOR )
   {
      // motor has not been started yet
      startingChannels--;
   }
   SET_STATE_L1( RUNNING );
   SET_STATE_L2( STOP_MOTOR );
   setTargetPosition( position );
   handleRunningState(); // to send evStop
}

bool RollerShutter::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      run();
   }
   else if ( event.isEvMessage() )
   {
      return handleRequest( event.isEvMessage()->getMessage() );
   }

   return false;
}

void RollerShutter::run()
{
   if ( inStartUp() )
   {
      setConfiguration( HwConfiguration::getRollerShutterConfiguration( id ) );
      setPosition( configuration->getPosition() );
      setTargetPosition( getPosition() );
      setToggleDirection( getPosition() ? TO_OPEN : TO_CLOSE );
      SET_STATE_L1( IDLE );
   }
   else if ( inIdle() )
   {
      hardware->setInverted( configuration->isInverted() );
      hardware->off();
      setSleepTime( NO_WAKE_UP );
   }
   else if ( inRunning() )
   {
      handleRunningState();
   }
}

bool RollerShutter::handleRequest( HACF* message )
{
   if ( !message->controlFrame.isCommand() )
   {
      return false;
   }
   HACF::ControlFrame& cf = message->controlFrame;
   Command* data = reinterpret_cast<Command*>( cf.getData() );
   if ( cf.isCommand( Command::SET_CONFIGURATION ) )
   {
      DEBUG_H1( ".setConfiguration()" );
      configuration->set( data->parameter.setConfiguration );
   }
   else if ( cf.isCommand( Command::SET_POSITION ) )
   {
      DEBUG_H1( ".setPosition()" );
      setPosition( data->parameter.position );
   }
   else if ( cf.isCommand( Command::MOVE_TO_POSITION ) )
   {
      DEBUG_H1( ".moveToPosition()" );
      cmdMoveToPosition( data->parameter.position );
   }
   else if ( cf.isCommand( Command::START ) )
   {
      DEBUG_H1( ".start()" );

      if ( data->parameter.direction == TOGGLE )
      {
         data->parameter.direction = toggleDirection;
      }
      if ( data->parameter.direction == TO_CLOSE )
      {
         toggleDirection = TO_OPEN;
         cmdMoveToPosition( HwConfiguration::RollerShutter::MAX_LEVEL );
      }
      else
      {
         toggleDirection = TO_CLOSE;
         cmdMoveToPosition( 0 );
      }
   }
   else if ( cf.isCommand( Command::STOP ) )
   {
      DEBUG_H1( ".stop()" );
      cmdStop();
   }
   else
   {
      Response response( getId(), *message );

      if ( cf.isCommand( Command::GET_CONFIGURATION ) )
      {
         DEBUG_H1( ".getConfiguration()" );
         configuration->get( response.setConfiguration().configuration );
      }
      else if ( cf.isCommand( Command::GET_STATUS ) )
      {
         DEBUG_H1( FSTR( ".getStatus()" ) );
         response.setStatus( getPosition() );
      }
      else
      {
         return false;
      }
      response.queue( getObject( message->header.getSenderId() ) );
   }
   return true;
}

void RollerShutter::handleRunningState()
{
   if ( ( subState == NOTIFY_RUNNING ) || ( subState == NOTIFY_STOPPED ) )
   {
      Response event( getId() );
      if ( subState == NOTIFY_RUNNING )
      {
         event.setDirection( hardware->isDirectionToOpen() ? TO_OPEN : TO_CLOSE );
         SET_STATE_L2( MOTOR_IS_RUNNING );
         event.queue();
      }
      else
      {
         event.setPosition( position );
         event.queue();

         SET_STATE_L1( IDLE );
         if ( position != targetPosition )
         {
            // restart hardware to targetPosition
            cmdMoveToPosition( targetPosition );
            return;
         }
         configuration->setPosition( position );
      }
   }

   if ( subState == START_MOTOR )
   {
      hardware->on();

      if ( startingChannels )
      {
         startingChannels--;
         DEBUG_M2( FSTR( "startingUnits-: " ), startingChannels );
      }

      setSleepTime( HwConfiguration::RollerShutter::DEFAULT_MOTOR_START_DELAY );
      SET_STATE_L2( NOTIFY_RUNNING );
   }
   else if ( subState == MOTOR_IS_RUNNING )
   {
      DEBUG_M2( FSTR( "target:" ), targetPosition );
      DEBUG_M2( FSTR( "position:" ), position );
      if ( position != targetPosition )
      {
         if ( hardware->isDirectionToOpen() )
         {
            if ( position > 0 )
            {
               position--;
            }
            setToggleDirection( TO_CLOSE );
            if ( targetPosition >= position )
            {
               SET_STATE_L2( STOP_MOTOR );
            }
         }
         else
         {
            if ( position < HwConfiguration::RollerShutter::MAX_LEVEL )
            {
               position++;
            }
            setToggleDirection( TO_OPEN );
            if ( targetPosition <= position )
            {
               SET_STATE_L2( STOP_MOTOR );
            }
         }
         setSleepTime( getPollTime() );
      }
      else
      {
         SET_STATE_L2( STOP_MOTOR );
      }
   }

   if ( subState == STOP_MOTOR )
   {
      hardware->off();
      SET_STATE_L2( NOTIFY_STOPPED );
      setSleepTime( HwConfiguration::RollerShutter::DEFAULT_MOTOR_START_DELAY );
   }

}

void RollerShutter::setPosition( uint8_t p_position )
{
   position = minimum<uint8_t>( p_position,
                                HwConfiguration::RollerShutter::MAX_LEVEL );
}

HwConfiguration::RollerShutter* RollerShutter::getConfiguration() const
{
   return configuration;
}

void RollerShutter::setConfiguration(
   HwConfiguration::RollerShutter* p_RollerShutter )
{
   configuration = p_RollerShutter;
}
