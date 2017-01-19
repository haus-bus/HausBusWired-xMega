/*
 * Counter.h
 *
 *  Created on: 22.10.2015
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_MonitoredDigitalInput_H
#define HwUnits_MonitoredDigitalInput_H

#include "HwUnits.h"
#include "HwConfiguration.h"
#include <MonitoredInputHw.h>

class MonitoredDigitalInput: public Reactive
{
public:

  enum State
  {
    LOW,
    HIGH,
    MANIPULATED_OPEN,
    MANIPULATED_SHORTEN,
    NOT_INITIALIZED,
  };

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
      HwConfiguration::MonitoredDigitalInput setConfiguration;
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


      EVENT_LOW = HACF::EVENTS_START,
      EVENT_HIGH,
      EVENT_MANIPULATED_OPEN,
      EVENT_MANIPULATED_SHORTEN,
    };

    union Parameter
    {
      HwConfiguration::MonitoredDigitalInput configuration;
      State state;
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

    inline void setStatus( State _state )
    {
      controlFrame.setDataLength(
          sizeof(getResponse()) + sizeof(getParameter().state) );
      setResponse( STATUS );
      getParameter().state = _state;
    }

    inline void setEvent( uint8_t event )
    {
      controlFrame.setDataLength( sizeof(getResponse()) );
      setResponse( event );
    }

    Parameter& setConfiguration();

    ////    Attributes    ////

  private:

    Parameter params;
  };

  ////    Constructors and destructors    ////

  MonitoredDigitalInput( PortPin _hardware );

  ////    Operations    ////

  bool handleRequest( HACF* message );

  virtual bool notifyEvent( const Event& event );

  State getCurrentState();

  inline void * operator new( size_t size )
  {
    return allocOnce( size );
  }

private:

  void run();

  void notifyNewState( State _state );

  ////    Additional operations    ////

public:

protected:

private:

  ////    Attributes    ////

public:

protected:

  static const uint8_t debugLevel;

private:

  MonitoredInputHw hardware;

  ////    Relations and components    ////

protected:

  HwConfiguration::MonitoredDigitalInput* configuration;

};

#endif
