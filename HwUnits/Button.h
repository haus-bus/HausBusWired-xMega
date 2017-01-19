/*
 * Button.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_Button_H
#define HwUnits_Button_H

#include "HwUnits.h"
#include "HwConfiguration.h"

class CriticalSection;

class Event;

class HACF;

class Led;

class Scheduler;

class evMessage;

class Button: public Reactive
{
public:

  class Command;

  class Response;

  enum State
  {
    RELEASED,
    PRESSED,
    CLICKED,
    CLICKED_PRESSED,
    HOLD
  };

  class Command
  {
  public:

    enum Commands
    {
      GET_CONFIGURATION = HACF::COMMANDS_START,
      SET_CONFIGURATION,
      ENABLE_EVENTS,
      GET_STATUS
    };

    struct EnableEvents
    {
      uint8_t enable;
      uint8_t disabledDuration;
    };

    union Parameter
    {
      HwConfiguration::Button setConfiguration;
      EnableEvents enableEvents;
    };

    enum EEnable
    {
      False = 0,
      True,
      Invert
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

      EVENT_COVERED = HACF::EVENTS_START,
      EVENT_CLICKED,
      EVENT_DOUBLE_CLICKED,
      EVENT_HOLD_START,
      EVENT_HOLD_END,
      EVENT_FREE,
    };

    union Parameter
    {
      HwConfiguration::Button configuration;
      uint8_t buttonEvent;
      uint8_t state;
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

    inline bool isCovered();

    Parameter& setConfiguration();

    inline void setEvent( uint8_t event );

    inline void setState( uint8_t _state );

    ////    Attributes    ////

  private:

    Parameter params;
  };

  ////    Constructors and destructors    ////

  Button( uint8_t _id );

  ////    Operations    ////

  bool handleRequest( HACF* message );

  virtual bool notifyEvent( const Event& event );

  inline void * operator new( size_t size );

  void tick();

  void updateState( uint8_t pressed );

private:

  void notifyNewState( HwConfiguration::Button::Event event ) const;

  ////    Additional operations    ////

public:

  HwConfiguration::Button* getConfiguration() const;

  void setConfiguration( HwConfiguration::Button* p_Button );

  Led* getFeedbackLed() const;

  void setFeedbackLed( Led* p_Led );

protected:

  inline static const uint8_t getDebugLevel()
  {
    return debugLevel;
  }

  inline uint8_t getDisabledDuration() const
  {
    return disabledDuration;
  }

  inline void setDisabledDuration( uint8_t p_disabledDuration )
  {
    disabledDuration = p_disabledDuration;
  }

private:

  inline HwConfiguration::Button::Event getEnabledEvents() const
  {
    return enabledEvents;
  }

  inline void setEnabledEvents( HwConfiguration::Button::Event p_enabledEvents )
  {
    enabledEvents = p_enabledEvents;
  }

  ////    Attributes    ////

public:


protected:

  static const uint8_t debugLevel;

  uint8_t disabledDuration;

private:

  HwConfiguration::Button::Event enabledEvents;

  ////    Relations and components    ////

protected:

  HwConfiguration::Button* configuration;

  Led* feedbackLed;

};

inline void * Button::operator new( size_t size )
{
  return allocOnce( size );
}

inline bool Button::Response::isCovered()
{
  return getResponse() == EVENT_COVERED;
}

inline void Button::Response::setEvent( uint8_t event )
{
  controlFrame.setDataLength( sizeof(getResponse()) );
  setResponse( event );
}

inline void Button::Response::setState( uint8_t _state )
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().state) );
  setResponse( STATUS );
  if ( (_state == RELEASED) || (_state == CLICKED) )
  {
    getParameter().state = RELEASED;
  }
  else
  {
    getParameter().state = PRESSED;
  }
}

#endif
