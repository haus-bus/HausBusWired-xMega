/*
 * IrReceiver.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_IrReceiver_H
#define HwUnits_IrReceiver_H

#include "HwUnits.h"
#include <DigitalInput.h>
#include <IrDecoder.h>
class CriticalSection;

class Event;

class HACF;

class PortPin;

class Scheduler;

class evMessage;

class IrReceiver: public Reactive
{
public:

  class Command;

  class Response;

  static const uint8_t TIMEOUT = 25;

  class Command
  {
  public:

    enum Commands
    {
      OFF = HACF::COMMANDS_START,
      ON
    };

    union Parameter
    {
      uint8_t dummy;
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
      EVENT_OFF = HACF::EVENTS_START,
      EVENT_ON,
      EVENT_CLICKED,
      EVENT_HOLD_START,
      EVENT_HOLD_END
    };

    struct IrData
    {
        uint16_t address;
        uint16_t command;
    };

    union Parameter
    {
      IrData data;
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

    inline void setData( const IrDecoder::Data& data );

    ////    Attributes    ////

  private:

    Parameter params;
  };

  ////    Constructors and destructors    ////

  IrReceiver( uint8_t _id, PortPin portPin );

  ////    Operations    ////

  virtual bool notifyEvent( const Event& event );

  inline void * operator new( size_t size );

  void run();

private:

  void checkForNewEvent();

  bool handleRequest( HACF* message );

  ////    Additional operations    ////

public:

  DigitalInput* getInputPin() const;

  IrDecoder* getIrDecoder() const;

private:

  inline static const uint8_t getDebugLevel()
  {
    return debugLevel;
  }

  inline uint8_t getReceiving() const
  {
    return receiving;
  }

  inline void setReceiving( uint8_t p_receiving )
  {
    receiving = p_receiving;
  }

  inline uint8_t getRepetitionFrameNumber() const
  {
    return repetitionFrameNumber;
  }

  inline void setRepetitionFrameNumber( uint8_t p_repetitionFrameNumber )
  {
    repetitionFrameNumber = p_repetitionFrameNumber;
  }

  ////    Attributes    ////

  static const uint8_t debugLevel;

  uint8_t receiving;

  uint8_t repetitionFrameNumber;

  ////    Relations and components    ////

protected:

  DigitalInput inputPin;

  IrDecoder irDecoder;

};

inline void * IrReceiver::operator new( size_t size )
{
  return allocOnce( size );
}

inline void IrReceiver::Response::setData( const IrDecoder::Data& data )
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().data) );
  getParameter().data.address = data.address;
  getParameter().data.command = data.command;
}

#endif

