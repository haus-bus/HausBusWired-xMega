/*
 * Counter.h
 *
 *  Created on: 22.10.2015
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_Counter_H
#define HwUnits_Counter_H

#include "HwUnits.h"
#include "HwConfiguration.h"

class Counter: public Reactive
{
public:

  enum ErrorCodes
  {
    CONFIGURATION_OUT_OF_MEMORY,

  };

  class Command
  {
  public:

    enum Commands
    {
      GET_CONFIGURATION = HACF::COMMANDS_START,
      SET_CONFIGURATION,
      GET_VALUE,
      SET_VALUE
    };

    union Parameter
    {
      HwConfiguration::Counter setConfiguration;
      uint32_t value;
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
      VALUE,
      STATUS,
    };

    union Parameter
    {
      HwConfiguration::Counter configuration;
      uint32_t value;
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

    inline void setValue( uint32_t value );

    inline void setStatus( uint32_t value );

    ////    Attributes    ////

  private:

    Parameter params;
  };

  ////    Constructors and destructors    ////

  Counter( uint8_t _id );

  ////    Operations    ////

  bool handleRequest( HACF* message );

  virtual bool notifyEvent( const Event& event );

  inline void * operator new( size_t size );

  void updateState( uint8_t newState );

private:

  void tick();

  ////    Additional operations    ////

public:

  HwConfiguration::Counter* getConfiguration() const;

  void setConfiguration( HwConfiguration::Counter* p_Button );

protected:

  inline static const uint8_t getDebugLevel()
  {
    return debugLevel;
  }

private:

  ////    Attributes    ////

public:

protected:

  static const uint8_t debugLevel;

private:

  HwConfiguration::Counter::Mode mode;

  uint16_t timeToReport;

  uint32_t value;

  uint32_t lastValue;



  ////    Relations and components    ////

protected:

  HwConfiguration::Counter* configuration;

};

inline void * Counter::operator new( size_t size )
{
  return allocOnce( size );
}

inline void Counter::Response::setValue( uint32_t value )
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().value) );
  setResponse( VALUE );
  getParameter().value = value;
}

inline void Counter::Response::setStatus( uint32_t value )
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().value) );
  setResponse( STATUS );
  getParameter().value = value;
}

#endif
