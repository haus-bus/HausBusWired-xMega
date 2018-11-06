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
   controlFrame.setDataLength( sizeof( getResponse() ) + sizeof( getParameter().configuration ) );
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
            ( SystemTime::S / Configuration::MAX_LEVEL )
            * configuration->closeTime );
      }
      else
      {
         hardware->setDirectionToOpen();
         setPollTime( ( SystemTime::S / Configuration::MAX_LEVEL ) * configuration->openTime );
      }
      SET_STATE_L2( START_MOTOR );
      startingChannels++;
      DEBUG_M2( FSTR( "startingUnits+: " ), startingChannels );
      setSleepTime( startingChannels * Configuration::DEFAULT_MOTOR_START_DELAY );
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
      setConfiguration( ConfigurationManager::getConfiguration<EepromConfiguration>( id ) );
      setPosition( configuration->position );
      setTargetPosition( getPosition() );
      setToggleDirection( getPosition() ? TO_OPEN : TO_CLOSE );
      SET_STATE_L1( IDLE );
   }
   else if ( inIdle() )
   {
      hardware->setInverted( configuration->getOptions().inverted );
      hardware->setIndependent( configuration->getOptions().independent );
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
         if ( getPosition() > Configuration::CLOSED_THRESHOLD )
         {
            // allow always moving at the boundaries
            setPosition( Configuration::CLOSED_THRESHOLD );
         }
         toggleDirection = TO_OPEN;
         cmdMoveToPosition( Configuration::MAX_LEVEL );
      }
      else
      {
         if ( getPosition() < Configuration::OPEN_THRESHOLD )
         {
            // allow always moving at the boundaries
            setPosition( Configuration::OPEN_THRESHOLD );
         }
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
         configuration->position = position;
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
      setSleepTime( Configuration::DEFAULT_MOTOR_START_DELAY );
      SET_STATE_L2( NOTIFY_RUNNING );
   }
   else if ( subState == MOTOR_IS_RUNNING )
   {
      DEBUG_M2( FSTR( "target:" ), targetPosition );
      DEBUG_M2( FSTR( "position:" ), position );
      if ( position != targetPosition )
      {
         positionTick();
         if ( hardware->isDirectionToOpen() )
         {
            setToggleDirection( TO_CLOSE );
            if ( targetPosition >= position )
            {
               SET_STATE_L2( STOP_MOTOR );
            }
         }
         else
         {
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
      setSleepTime( Configuration::DEFAULT_MOTOR_START_DELAY );
   }

}

void RollerShutter::setPosition( uint8_t p_position )
{
   position = minimum<uint8_t>( p_position, Configuration::MAX_LEVEL );
}

void RollerShutter::positionTick()
{
   if ( hardware->isDirectionToOpen() )
   {
      if ( position > 0 )
      {
         position--;
      }
   }
   else
   {
      if ( position < Configuration::MAX_LEVEL )
      {
         position++;
      }
   }

}