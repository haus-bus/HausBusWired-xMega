/*
 * BaseSensorUnit.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_BaseSensorUnit_H
#define HwUnits_BaseSensorUnit_H

#include "HwUnits.h"
#include "HwConfiguration.h"

class CriticalSection;

class HACF;

class Scheduler;

class evMessage;

class BaseSensorUnit: public Reactive
{
public:

  class Command;

  class Response;

  struct Status
  {
    int8_t value;
    uint8_t centiValue;
    uint8_t lastEvent;
  };

  static const uint8_t MAX_ERRORS = 10;

  class Command
  {
  public:

    enum Commands
    {
      GET_CONFIGURATION = HACF::COMMANDS_START,
      SET_CONFIGURATION,
      GET_STATUS,
    };

    union Parameter
    {
      HwConfiguration::SensorUnit setConfiguration;
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

      EVENT_BELOW = HACF::EVENTS_START,
      EVENT_IN_RANGE,
      EVENT_ABOVE,
    };

    union Parameter
    {
      HwConfiguration::SensorUnit configuration;
      Status status;
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

    inline void setEvent( uint8_t value )
    {
      controlFrame.setDataLength( 1 );
      setResponse( value );
    }

    Parameter& setConfiguration();

    void setStatus( Status value );

    ////    Attributes    ////

  private:
    Parameter params;

  };

  ////    Constructors and destructors    ////

  inline BaseSensorUnit() :
      timeToReport( 1 ), currentEvent( 0 ), lastEvent( 0 ), errorCounter( 0 )
  {
    configuration = NULL;
  }

  ////    Operations    ////

  void notifyNewValue( Status newStatus );

protected:

  bool handleRequest( HACF* message );

  ////    Additional operations    ////

public:

  inline HwConfiguration::SensorUnit* getConfiguration() const
  {
    return configuration;
  }

  inline void setConfiguration( HwConfiguration::SensorUnit* p_SensorUnit )
  {
    configuration = p_SensorUnit;
  }

protected:

  inline static const uint8_t getDebugLevel()
  {
    return debugLevel;
  }

  inline Status getLastStatus() const
  {
    return lastStatus;
  }

  inline void setLastStatus( Status p_lastStatus )
  {
    lastStatus = p_lastStatus;
  }

  inline uint8_t getTimeToReport() const
  {
    return timeToReport;
  }

  inline void setTimeToReport( uint8_t p_timeToReport )
  {
    timeToReport = p_timeToReport;
  }

  ////    Attributes    ////

  static const uint8_t debugLevel;

  Status lastStatus;

  uint8_t timeToReport;

  uint8_t currentEvent;

  uint8_t lastEvent;

  uint8_t errorCounter;

  ////    Relations and components    ////

  HwConfiguration::SensorUnit* configuration;

};

#endif
