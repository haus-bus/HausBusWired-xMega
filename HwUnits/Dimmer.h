/*
 * Dimmer.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_Dimmer_H
#define HwUnits_Dimmer_H

#include "HwUnits.h"
#include "HwConfiguration.h"
#include <IDimmerHw.h>
class CriticalSection;

class Event;

class HACF;

class IDimmerHw;

class Scheduler;

class evMessage;

class Dimmer: public Reactive
{
public:

  class Command;

  class Response;

  enum SubStates
  {
    NO_COMMAND,
    DIMMING
  };

  enum Direction
  {
    TO_DARK = -1,
    TOGGLE = 0,
    TO_LIGHT = 1
  };

  static const uint16_t TIME_BASE = SystemTime::S;

  class Command
  {
  public:

    enum Commands
    {
      GET_CONFIGURATION = HACF::COMMANDS_START,
      SET_CONFIGURATION,
      SET_BRIGHTNESS,
      START,
      STOP,
      GET_STATUS,
    };

    struct SetBrightness
    {
      uint8_t brightness;
      uint16_t duration;
    };

    struct Start
    {
      int8_t direction;
    };

    union Parameter
    {
      SetBrightness setBrightness;
      HwConfiguration::Dimmer setConfiguration;
      Start start;
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

      EVENT_OFF = HACF::EVENTS_START,
      EVENT_ON,
      EVENT_START,

      EVENT_ERROR = HACF::EVENTS_END
    };

    union Parameter
    {
      HwConfiguration::Dimmer configuration;
      uint8_t brightness;
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

    void setBrightness( uint16_t brightness );

    Parameter& setConfiguration();

    void setDirection( uint8_t direction );

    void setStatus( uint16_t brightness );

    ////    Attributes    ////

  private:

    Parameter params;

  };

  ////    Constructors and destructors    ////

  Dimmer( uint8_t _id, IDimmerHw* _hardware );

  ////    Operations    ////

  virtual bool notifyEvent( const Event& event );

  inline void * operator new( size_t size );

  void run();

private:

  void cmdSetBrightness( Command::SetBrightness& parameter, uint8_t changeTime =
                             0 );

  void cmdStart( const Command::Start& parameter );

  void configureHw();

  int16_t getDimmingRangeEnd() const;

  int16_t getDimmingRangeStart() const;

  inline uint16_t getStepWidth( uint8_t time = 0 );

  bool handleRequest( HACF* message );

  void handleRunningState();

  void handleSuspend();

  void notifyError( uint8_t errorCode );

  ////    Additional operations    ////

public:

  HwConfiguration::Dimmer* getConfiguration() const;

  void setConfiguration( HwConfiguration::Dimmer* p_Dimmer );

  IDimmerHw* getHardware() const;

  void setHardware( IDimmerHw* p_IDimmerHw );

private:

  inline uint16_t getBrightness() const
  {
    return brightness;
  }

  inline void setBrightness( uint16_t p_brightness )
  {
    brightness = p_brightness;
  }

  inline static const uint8_t getDebugLevel()
  {
    return debugLevel;
  }

  inline void setDimmingRangeEnd( uint8_t p_dimmingRangeEnd )
  {
    dimmingRangeEnd = p_dimmingRangeEnd;
  }

  inline void setDimmingRangeStart( uint8_t p_dimmingRangeStart )
  {
    dimmingRangeStart = p_dimmingRangeStart;
  }

  inline int16_t getDirection() const
  {
    return direction;
  }

  inline void setDirection( int16_t p_direction )
  {
    direction = p_direction;
  }

  inline uint16_t getDuration() const
  {
    return duration;
  }

  inline void setDuration( uint16_t p_duration )
  {
    duration = p_duration;
  }

  inline SubStates getSubState() const
  {
    return subState;
  }

  inline void setSubState( SubStates p_subState )
  {
    subState = p_subState;
  }

  inline uint16_t getTargetBrightness() const
  {
    return targetBrightness;
  }

  inline void setTargetBrightness( uint16_t p_targetBrightness )
  {
    targetBrightness = p_targetBrightness;
  }

  ////    Attributes    ////

  int16_t brightness;

  static const uint8_t debugLevel;

  uint8_t dimmingRangeStart;

  uint8_t dimmingRangeEnd;

  int16_t direction;

  uint16_t duration;

  SubStates subState;

  int16_t targetBrightness;

  ////    Relations and components    ////

protected:

  HwConfiguration::Dimmer* configuration;

  IDimmerHw* hardware;

};

inline void * Dimmer::operator new( size_t size )
{
  return allocOnce( size );
}

inline uint16_t Dimmer::getStepWidth( uint8_t time )
{
  if ( time == 0 ) time = configuration->getFadingTime();
  if ( time == 0 ) time = 1;

  return 10000 / HwConfiguration::Dimmer::TIME_FACTOR / time;
}

#endif
