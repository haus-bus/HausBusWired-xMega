/*
 * RollerShutter.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_RollerShutter_H
#define HwUnits_RollerShutter_H

#include "HwUnits.h"
#include "HwConfiguration.h"
#include <RollerShutterHw.h>

class CriticalSection;

class Event;

class HACF;

class Scheduler;

class evMessage;

class RollerShutter: public Reactive
{
public:

  class Command;

  class Response;

  enum SubStates
  {
    NO_COMMAND,
    START_MOTOR,
    MOTOR_IS_RUNNING,
    STOP_MOTOR,
    NOTIFY_RUNNING,
    NOTIFY_STOPPED
  };

  enum Direction
  {
    TO_OPEN = -1,
    TOGGLE = 0,
    TO_CLOSE = 1
  };

  class Command
  {
  public:

    enum Commands
    {
      GET_CONFIGURATION = HACF::COMMANDS_START,
      SET_CONFIGURATION,
      MOVE_TO_POSITION,
      START,
      STOP,
      GET_STATUS,
      SET_POSITION
    };

    union Parameter
    {
      uint8_t position;
      int8_t direction;
      HwConfiguration::RollerShutter setConfiguration;
    };

    ////    Operations    ////

    inline Parameter& getParameter()
    {
      return parameter;
    }

    ////    Additional operations    ////

    inline uint8_t getCommand() const
    {
      return command;
    }

    inline void setCommand( uint8_t p_command )
    {
      command = p_command;
    }

    inline void setParameter( Parameter p_parameter )
    {
      parameter = p_parameter;
    }

    ////    Attributes    ////

    uint8_t command;

    Parameter parameter;
  };

  class Response: public IResponse
  {
  public:

    enum Responses
    {
      CONFIGURATION = HACF::RESULTS_START,
      STATUS,

      EVENT_CLOSED = HACF::EVENTS_START,
      EVENT_START,
      EVENT_OPEN,
    };

    union Parameter
    {
      HwConfiguration::RollerShutter configuration;
      uint8_t position;
      int8_t direction;
      uint8_t status;
    };

    ////    Constructors and destructors    ////

    inline Response( uint16_t id ) :
        IResponse( id )
    {
    }

    inline Response( uint16_t id, const HACF& message ) :
        IResponse( id, message )
    {
    }

    ////    Operations    ////

    inline Parameter& getParameter()
    {
      return *reinterpret_cast<Parameter*>( IResponse::getParameter() );
    }

    Parameter& setConfiguration();

    void setDirection( uint8_t direction );

    void setPosition( uint8_t position );

    void setStatus( uint8_t position );

    ////    Attributes    ////

  private:

    Parameter params;
  };

  ////    Constructors and destructors    ////

  RollerShutter( uint8_t _id, RollerShutterHw* _hardware );

  ////    Operations    ////

  void cmdMoveToPosition( uint8_t target );

  void cmdStop();

  virtual bool notifyEvent( const Event& event );

  inline void * operator new( size_t size );

  void run();

private:

  bool handleRequest( HACF* message );

  void handleRunningState();

  void setPosition( uint8_t p_position );

  void positionTick();

  ////    Additional operations    ////

public:

  HwConfiguration::RollerShutter* getConfiguration() const;

  void setConfiguration( HwConfiguration::RollerShutter* p_RollerShutter );

  inline RollerShutterHw* getHardware() const
  {
    return hardware;
  }

  inline void setHardware( RollerShutterHw* p_RollerShutterHw )
  {
    hardware = p_RollerShutterHw;
  }

private:

  inline uint8_t getPosition() const
  {
    return position;
  }

  inline static uint8_t getStartingChannels()
  {
    return startingChannels;
  }

  inline static void setStartingChannels( uint8_t p_startingChannels )
  {
    startingChannels = p_startingChannels;
  }

  inline static const uint8_t getDebugLevel()
  {
    return debugLevel;
  }

  inline uint8_t getTargetPosition() const
  {
    return targetPosition;
  }

  inline void setTargetPosition( uint8_t p_targetPosition )
  {
    targetPosition = minimum<uint8_t>( p_targetPosition,
                                 HwConfiguration::RollerShutter::MAX_LEVEL );
  }

  inline uint16_t getPollTime() const
  {
    return pollTime;
  }

  inline void setPollTime( uint16_t p_pollTime )
  {
    pollTime = p_pollTime;
  }

  inline uint8_t getSubState() const
  {
    return subState;
  }

  inline void setSubState( uint8_t p_subState )
  {
    subState = p_subState;
  }

  inline uint8_t getToggleDirection() const
  {
    return toggleDirection;
  }

  inline void setToggleDirection( uint8_t p_toggleDirection )
  {
    toggleDirection = p_toggleDirection;
  }

  ////    Attributes    ////

  static uint8_t startingChannels;

  static const uint8_t debugLevel;

  uint8_t subState;

  uint8_t position;

  uint8_t targetPosition;

  uint16_t pollTime;

  uint8_t toggleDirection;

  ////    Relations and components    ////

protected:

  HwConfiguration::RollerShutter* configuration;

  RollerShutterHw* hardware;

};

inline void * RollerShutter::operator new( size_t size )
{
  return allocOnce( size );
}

#endif
