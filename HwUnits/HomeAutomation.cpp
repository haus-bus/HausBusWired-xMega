/*
 * HomeAutomation.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "HomeAutomation.h"
#include "DebugOptions.h"
#include <EventPkg/EventPkg.h>
#include <Gateway.h>
#include <Protocols/HACF.h>
#include <PersistentRules.h>
#include <RuleEngine.h>
#include <Scheduler.h>
#include <Time/Calender.h>

extern uint16_t getUnusedMemory();

const uint8_t HomeAutomation::debugLevel( DEBUG_LEVEL_OFF );

Timestamp HomeAutomation::lastMinuteTick;

Timestamp HomeAutomation::lastMemoryReportTime;

uint16_t HomeAutomation::LogicalUnitGroup::state[MAX_GROUPS];

uint8_t HomeAutomation::LogicalUnitGroup::setState(
   const HomeAutomationInterface::Command::SetUnitGroupState& params )
{
   uint8_t idx = params.index;
   uint16_t preState = state[idx];
   uint16_t triggerLevel = ( 1 << params.triggerBits ) - 1;

   if ( params.state )
   {
      state[idx] |= ( 1 << params.member );
   }
   else
   {
      state[idx] &= ~( 1 << params.member );
   }

   if ( preState && !state[idx] )
   {
      return HomeAutomationInterface::Response::EVENT_GROUP_OFF;
   }
   if ( preState != state[idx] )
   {
      if ( state[idx] == triggerLevel )
      {
         return HomeAutomationInterface::Response::EVENT_GROUP_ON;
      }
      if ( !preState || ( preState == triggerLevel ) )
      {
         return HomeAutomationInterface::Response::EVENT_GROUP_UNDEFINED;
      }
   }
   return 0;
}

void HomeAutomation::MyEvent::setTimeEvent()
{
   controlFrame.setDataLength( 4 );
   setResponse( EVENT_TIME );
   uint8_t* weekTime = (uint8_t*) getParameter();
   weekTime[0] = Calender::now.getMinute();
   weekTime[1] = Calender::now.getHour();
   weekTime[2] = Calender::now.getDayOfWeek();
}

bool HomeAutomation::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      run();
   }
   else if ( event.isEvTime() )
   {
      WeekTime cuurentWeekTime = Calender::getCurrentWeekTime();
      MyEvent myEvent;

      bool isSunRise = cuurentWeekTime.isSunRise();
      if ( isSunRise || cuurentWeekTime.isSunSet() )
      {
         myEvent.setResponse(
            isSunRise ? MyEvent::EVENT_DAY : MyEvent::EVENT_NIGHT );
         myEvent.queue();
         DEBUG_H1( ( isSunRise ? FSTR( ".evDay" ) : FSTR( ".evNight" ) ) );
      }
      myEvent.setTimeEvent();
      myEvent.queue();
      DEBUG_H2( FSTR( ".evTime 0x" ), cuurentWeekTime.get() );

// event.setTime( cuurentWeekTime.get() );
// event.queue();
   }
   else if ( event.isEvGatewayMessage() )
   {
      HACF* message = event.isEvGatewayMessage()->getMessage();

      DEBUG_H1( FSTR( ".evGatewayMessage" ) );
      DEBUG_M2( FSTR( "sender   " ), message->header.getSenderId() );

      if ( !message->isForThisDeviceOnly()
         && !( ( message->controlFrame.receiverId.getId() == 0x0000B001UL )
             && ( message->header.getSenderId() == 0xB001 ) ) )
      {
         uint8_t i = Gateway::MAX_GATEWAYS;
         while ( i-- )
         {
            Gateway* gateway = Gateway::getItsGateway( i );
            if ( gateway )
            {
               if ( gateway->getId() != message->header.getSenderId() )
               {
                  if ( evGatewayMessage( gateway, message ).queue() )
                  {
                     message->header.incrementReferenceCount();
                     DEBUG_M2( FSTR( "send to gateway " ), gateway->getId() );
                  }
                  else
                  {
                     WARN_1( FSTR( "EventQueue full " ) );
                  }
               }
            }
         }
      }

      DEBUG_M1( FSTR( ".routeMsg" ) );
      if ( message->controlFrame.isRelevantForComponent() )
      {
         EventDrivenUnit* receiver = getObject(
            message->controlFrame.receiverId.getObjectId() );
         if ( receiver )
         {
            DEBUG_M2( FSTR( "send to receiver " ), receiver->getId() );
            evMessage( receiver, message ).send();
         }
         else if ( message->controlFrame.receiverId.getInstanceId() == 0 )
         {
            DEBUG_M1( FSTR( "searchReceiver" ) );
            uint8_t startIndex = 0;
            uint8_t classId = message->controlFrame.receiverId.getClassId();
            receiver = getNextOfClass( classId, startIndex );
            while ( receiver )
            {
               DEBUG_M2( FSTR( "send to receiver " ), receiver->getId() );
               evMessage( receiver, message ).send();
               receiver = getNextOfClass( classId, startIndex );
            }
         }
         else
         {
            WARN_2( getId(), ":receiver not found" );
         }
      }
      delete message;
      return true;
   }
   else if ( event.isEvMessage() )
   {
      DEBUG_H1( FSTR( ".evMessage" ) );
      return handleRequest( event.isEvMessage()->getMessage() );
   }

   return false;
}

void HomeAutomation::run()
{
   if ( inFatalError() )
   {
      ResetSystem::reset();
   }

   HomeAutomationInterface::Response event( getId() );
   HwConfiguration::HomeAutomation& conf
      = HwConfiguration::HomeAutomation::instance();

   if ( inStartUp() )
   {
      // notify server that new module has started
      event.setStarted( ResetSystem::getSources() );
      event.queue( this );

// if ( !ResetSystem::getSources() )
// {
// event.setErrorCode( HomeAutomationInterface::ErrorCode::NULL_POINTER );
// event.queue( this );
// }
      if ( ResetSystem::isBrownOutReset() )
      {
         event.setErrorCode( HomeAutomationInterface::ErrorCode::LOW_VOLTAGE );
         event.queue( this );
      }
      if ( ResetSystem::isWatchDogReset() )
      {
         event.setErrorCode( HomeAutomationInterface::ErrorCode::WATCHDOG );
         event.queue( this );
      }
      checkPersistentRules();
      ResetSystem::clearSources();
      setMainState( IDLE );
   }
   else
   {
      uint16_t ticksPerMinute = getMinuteTicks();

      SystemTime::time_t reportTime
         = (SystemTime::time_t) conf.getReportMemoryStatusTime() * ticksPerMinute;

      if ( reportTime && lastMemoryReportTime.elapsed( reportTime ) )
      {
         lastMemoryReportTime = Timestamp();
         event.setUnusedMemory();
         event.queue( this );
      }

      setSleepTime( SystemTime::S );
      /*
         HomeAutomation::Configuration conf;
         uint8_t errorCode = HomeAutomation::Configuration::instance().get( conf );
         if ( errorCode )
         {
         HACF::setDeviceId(
         HomeAutomation::Configuration::instance().getDeviceId() );
         event.setErrorCode(
         HomeAutomationInterface::ErrorCode::CHECKSUM_ERROR + errorCode );
         event.queue( this );
         }
       */
   }
}

void HomeAutomation::checkPersistentRules()
{
   if ( !PersistentRules::instance().init() )
   {
      IResponse event( getId() );
      event.setErrorCode(
         HomeAutomationInterface::ErrorCode::INVALID_RULE_TABLE );
      event.queue( this );
   }
}

void HomeAutomation::cmdGetRemoteObjects(
   HomeAutomationInterface::Response& response )
{
   DEBUG_H2( getId(), FSTR( ".getRemoteObjects()" ) );

   Reactive** list = getObjectList();
   uint8_t index = 0;
   uint8_t numOfObjects = 0;

   while ( list[index] )
   {
      if ( list[index]->hasRemoteAccess() )
      {
         DEBUG_M3( (uint16_t )list[index], FSTR( " object 0x" ),
                   list[index]->getId() );
         response.getParameter().remoteObjects[numOfObjects++]
            = list[index]->getId();
      }
      if ( index++ >= getMaxObjectListSize() )
      {
         IResponse event( getId() );
         event.setErrorCode(
            HomeAutomationInterface::ErrorCode::MAX_OBJECTS_REACHED );
         event.queue( this );
         break;
      }
   }
   response.controlFrame.setDataLength(
      sizeof( response.getResponse() )
      + sizeof( response.getParameter().remoteObjects[0] ) * numOfObjects );
   response.setResponse( HomeAutomationInterface::Response::REMOTE_OBJECTS );
}

void HomeAutomation::cmdReadMemory(
   HomeAutomationInterface::Command::ReadMemory& parameter,
   HomeAutomationInterface::Response& response )
{
   DEBUG_H2( getId(), FSTR( ".readMemory()" ) );
   uint8_t* dest = response.setReadMemory( parameter.address, parameter.length );

   Stream::Status status = Stream::SUCCESS;
   if ( parameter.address & HomeAutomationInterface::DATA_SECTION_MASK )
   {
      uint8_t* source = reinterpret_cast<uint8_t*>( parameter.address & 0xFFFF );
      uint16_t length = parameter.length;
      while ( length-- )
      {
         dest[length] = source[length];
      }
   }
   else
   {
      status = Flash::read( parameter.address, dest, parameter.length );
   }

   if ( status != Stream::SUCCESS )
   {
      response.setMemoryStatus( status );
   }

}

void HomeAutomation::cmdWriteRules(
   HomeAutomationInterface::Command::WriteRules& parameter,
   uint16_t dataLength, HomeAutomationInterface::Response& response )
{
   DEBUG_H1( FSTR( ".writeRules()" ) );
   RuleEngine::disable();
   response.setMemoryStatus(
      HomeAutomationHw::writeRules( parameter.offset, parameter.data,
                                    dataLength ) );
   if ( dataLength < APP_SECTION_PAGE_SIZE )
   {
      checkPersistentRules();
   }

}

bool HomeAutomation::handleRequest( HACF* message )
{
   bool consumed = true;

   HACF::ControlFrame& cf = message->controlFrame;
   HomeAutomationInterface::Command* data
      = reinterpret_cast<HomeAutomationInterface::Command*>( cf.data );
   HomeAutomationInterface::Response response( getId() );

   if ( cf.isCommand() )
   {
      if ( cf.isCommand( HomeAutomationInterface::Command::RESET ) )
      {
         DEBUG_H1( FSTR( ".reset()" ) );
         setMainState( FATAL_ERROR );
         setSleepTime( TIME_TO_RESET );
      }
      else if ( cf.isCommand( HomeAutomationInterface::Command::SET_RULE_STATE ) )
      {
         DEBUG_H1( FSTR( ".setRuleState()" ) );
         uint8_t idx = data->parameter.setRuleState.index;
         if ( idx < RuleEngine::getNumOfRules() )
         {
            RuleEngine::getRules()[idx].setState(
               data->parameter.setRuleState.state );
         }
      }
      else if ( cf.isCommand(
                   HomeAutomationInterface::Command::TRIGGER_RULE_ELEMENT ) )
      {
         DEBUG_H1( FSTR( ".triggerRuleElement()" ) );
         uint8_t idx = data->parameter.triggerRuleElement.idxRule;
         if ( idx < RuleEngine::getNumOfRules() )
         {
            RuleEngine::getRules()[idx].triggerElement(
               data->parameter.triggerRuleElement.idxElement );
         }
      }
      else if ( cf.isCommand(
                   HomeAutomationInterface::Command::SET_UNIT_GROUP_STATE ) )
      {
         DEBUG_H1( FSTR( ".setUnitGroupState()" ) );
         uint8_t event = LogicalUnitGroup::setState(
            data->parameter.setUnitGroupState );
         if ( event )
         {
            response.setUnitGroupEvent( event,
                                        data->parameter.setUnitGroupState.index );
            response.queue( this );
         }
      }
      else if ( cf.isCommand(
                   HomeAutomationInterface::Command::SET_DEBUG_OPTIONS ) )
      {
         DEBUG_H1( FSTR( ".setDebugOptions()" ) );
         DebugOptions::set( data->parameter.debugOptions );
      }
      else if ( cf.isCommand( HomeAutomationInterface::Command::SET_SUN_TIMES ) )
      {
         DEBUG_H1( FSTR( ".setSunTimes()" ) );
         WeekTime::sunRise.set(
            WeekTime::WEEK_DAY_MASK | data->parameter.sunTimes.sunRise );
         WeekTime::sunSet.set(
            WeekTime::WEEK_DAY_MASK | data->parameter.sunTimes.sunSet );
      }
      else if ( cf.isCommand( HomeAutomationInterface::Command::SET_TIME ) )
      {
         DEBUG_H1( FSTR( ".setTime()" ) );
         int16_t timeDifference = WeekTime( data->parameter.time ).distanceToNow();

         if ( abs( timeDifference ) > 10 )
         {

            Calender::now.setWeekTime( WeekTime( data->parameter.time ) );
            lastMinuteTick = Timestamp();
         }
         else if ( timeDifference )
         {
            HwConfiguration::HomeAutomation& conf
               = HwConfiguration::HomeAutomation::instance();
            int16_t timeCorrection = conf.getTimeCorrectionValue();
            timeCorrection += timeDifference;
            if ( timeCorrection > 0xFF )
            {
               timeCorrection = 0xFF;
            }
            else if ( timeCorrection < 0 )
            {
               timeCorrection = 0;
            }
            conf.setTimeCorrectionValue( timeCorrection );

            lastMinuteTick = Timestamp();
            lastMinuteTick += ( (int32_t) timeDifference * getMinuteTicks() );

            if ( abs( timeDifference ) > 2 )
            {
               response.setTimeDifference( timeDifference );
               response.queue( this );
            }
         }
      }
      else
      {
         response.setupForResult( *message );
         if ( cf.isCommand( HomeAutomationInterface::Command::GET_MODULE_ID ) )
         {
            DEBUG_H1( FSTR( ".getModuleId()" ) );
            if ( !HomeAutomationHw::getModuleId( data->parameter.getModuleId.index,
                                                 response.setModuleId() ) )
            {
               response.setErrorCode(
                  HomeAutomationInterface::ErrorCode::MODULE_NOT_EXISTS );
            }
         }
         else if ( cf.isCommand( HomeAutomationInterface::Command::GET_TIME ) )
         {
            DEBUG_H1( FSTR( ".getTime()" ) );
            response.setTime( Calender::getCurrentWeekTime().get() );
         }
         else if ( cf.isCommand(
                      HomeAutomationInterface::Command::GET_RULE_STATE ) )
         {
            DEBUG_H1( FSTR( ".getRuleState()" ) );
            uint8_t idx = data->parameter.index;
            if ( idx < RuleEngine::getNumOfRules() )
            {
               response.setRuleState( idx, RuleEngine::getRules()[idx].getState() );
            }
            else
            {
               response.setErrorCode(
                  HomeAutomationInterface::ErrorCode::SYNTAX_ERROR );
            }
         }
         else if ( cf.isCommand(
                      HomeAutomationInterface::Command::SET_CONFIGURATION ) )
         {
            if ( cf.getDataLength()
                 == ( HwConfiguration::HomeAutomation::SIZEOF + 1 ) )
            {
               DEBUG_H1( FSTR( ".setConfiguration()" ) );
               HwConfiguration::HomeAutomation conf;
               HwConfiguration::HomeAutomation::instance().get( conf );
               memcpy( &conf, &data->parameter.setConfiguration,
                       HwConfiguration::HomeAutomation::SIZEOF );
               HwConfiguration::HomeAutomation::instance().set( conf );
               lastMemoryReportTime = Timestamp();
               setSleepTime( WAKE_UP );
               if ( HACF::deviceId
                    != HwConfiguration::HomeAutomation::instance().getDeviceId() )
               {
                  // change deviceId only after reset
                  // HACF::deviceId = HomeAutomationHw::Configuration::instance().getDeviceId();
                  response.setDeviceId(
                     HwConfiguration::HomeAutomation::instance().getDeviceId() );
               }
               else
               {
                  return true;
               }
            }
            else
            {
               response.setErrorCode(
                  HomeAutomationInterface::ErrorCode::SYNTAX_ERROR );
            }
         }
         else if ( cf.isCommand(
                      HomeAutomationInterface::Command::GENERATE_RANDOM_DEVICE_ID ) )
         {
            if ( cf.getDataLength() == 1 )
            {
               uint16_t deviceId = SystemTime::now() & 0x7FFF;
               HwConfiguration::HomeAutomation::instance().setDeviceId( deviceId );
               response.setDeviceId( deviceId );
            }
            else
            {
               response.setErrorCode(
                  HomeAutomationInterface::ErrorCode::SYNTAX_ERROR );
            }
         }
         else if ( cf.isCommand(
                      HomeAutomationInterface::Command::GET_REMOTE_OBJECTS ) )
         {
            cmdGetRemoteObjects( response );
         }
         else if ( cf.isCommand(
                      HomeAutomationInterface::Command::GET_UNUSED_MEMORY ) )
         {
            DEBUG_H1( FSTR( ".getUnusedMemory()" ) );
            response.setUnusedMemory();
         }
         else if ( cf.isCommand(
                      HomeAutomationInterface::Command::GET_CONFIGURATION ) )
         {
            DEBUG_H1( FSTR( ".getConfiguration()" ) );
            HwConfiguration::HomeAutomation::instance().get(
               response.setConfiguration( getFckE() ) );
         }
         else if ( cf.isCommand( HomeAutomationInterface::Command::READ_MEMORY ) )
         {
            cmdReadMemory( data->parameter.readMemory, response );
         }
         else if ( cf.isCommand( HomeAutomationInterface::Command::WRITE_MEMORY ) )
         {
            DEBUG_H1( FSTR( ".writeMemory()" ) );
            response.setMemoryStatus( Stream::LOCKED );
         }
         else if ( cf.isCommand( HomeAutomationInterface::Command::WRITE_RULES ) )
         {
            cmdWriteRules(
               data->parameter.writeRules,
               cf.getDataLength() - sizeof( data->command )
               - sizeof( data->parameter.writeRules.offset ),
               response );
         }
         else if ( cf.isCommand( HomeAutomationInterface::Command::READ_RULES ) )
         {
            DEBUG_H1( FSTR( ".readRules()" ) );
            HomeAutomationInterface::Command::ReadRules* parameter
               = &data->parameter.readRules;
            HomeAutomationHw::readRules(
               parameter->offset,
               response.setReadRules( parameter->offset, parameter->length ),
               parameter->length );
         }
         else if ( cf.isCommand( HomeAutomationInterface::Command::PING ) )
         {
            DEBUG_H1( FSTR( ".ping()" ) );
            response.setPong();
         }
         else
         {
            return false;
         }
         response.queue( getObject( message->header.getSenderId() ) );
      }
   }
   else
   {
      // consume all events
      DEBUG_H1( FSTR( ".forward event to RuleEngine" ) );
      RuleEngine::notifyEvent( message->controlFrame );
      return true;
   }

   return consumed;
}

HomeAutomationInterface::Response* HomeAutomation::getErrorEvent() const
{
   return (HomeAutomationInterface::Response*) &errorEvent;
}

uint16_t HomeAutomation::getMinuteTicks() const
{
   return MINUTE_TICKS
          + HwConfiguration::HomeAutomation::instance().getTimeCorrectionValue();
}
