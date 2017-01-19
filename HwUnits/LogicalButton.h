/*
 * LogicalButton.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_LogicalButton_H
#define HwUnits_LogicalButton_H

#include "HwUnits.h"
#include "HwConfiguration.h"
class Button;

class CriticalSection;

class Event;

class HACF;

class Led;

class Scheduler;

class evMessage;

class LogicalButton: public Reactive
{
public:

  class Command;

  class ErrorCode;

  class Response;

  class Command
  {
  public:

    enum Commands
    {
      GET_CONFIGURATION = HACF::COMMANDS_START,
      SET_CONFIGURATION,
      OFF,
      ON,
      BLINK,
      GET_STATUS,
      SET_MIN_BRIGHTNESS,
      RESERVED_2,
      RESERVED_3,
      RESERVED_4,
      RESERVED_5,
      SET_BUTTON_CONFIGURATION,
      SET_LED_CONFIGURATION
    };

    union Parameter
    {
      HwConfiguration::LogicalButton setConfiguration;
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

    };

    union Parameter
    {
      HwConfiguration::LogicalButton configuration;
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

    void setStatus( uint8_t status );

    ////    Attributes    ////

  private:

    Parameter params;
  };

  class ErrorCode
  {
  public:

    enum Errors
    {
      NO_LED,
      NO_BUTTON,
    };
  };

  ////    Constructors and destructors    ////

  LogicalButton( uint8_t _id );

  ////    Operations    ////

  void clearObjectList();

  uint8_t getLedStatus();

  virtual bool notifyEvent( const Event& event );

  inline void * operator new( size_t size );

  void routeMessageToButton( const Event& event );

  void routeMessageToLed( const Event& event );

  void updateObjectList();

protected:

  bool handleRequest( const Event& event );

  ////    Additional operations    ////

public:

  void addButton( Button* p_Button );

  HwConfiguration::LogicalButton* getConfiguration() const;

  void setConfiguration( HwConfiguration::LogicalButton* p_LogicalButton );

  void addLed( Led* p_Led );

protected:

  inline static const uint8_t getDebugLevel()
  {
    return debugLevel;
  }

  ////    Attributes    ////

  static const uint8_t debugLevel;

  ////    Relations and components    ////

  Button* button[HwConfiguration::LogicalButton::MAX_OBJECTS];

  HwConfiguration::LogicalButton* configuration;

  Led* led[HwConfiguration::LogicalButton::MAX_OBJECTS];

};

inline void * LogicalButton::operator new( size_t size )
{
  return allocOnce( size );
}

#endif
