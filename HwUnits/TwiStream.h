/*
 * TwiStream.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_TwiStream_H
#define HwUnits_TwiStream_H

#include "HwUnits.h"
#include <Gateway.h>
#include <Stream.h>
#include <Time/Timestamp.h>
#include <DigitalOutput.h>
#include <Peripherals/Twi.h>

class CriticalSection;
class Event;
class HACF;
class IResponse;
class ResetSystem;
class Scheduler;
class evMessage;
class string;

class TwiStream: public Gateway
{
public:

  class Command;
  class Response;

  struct Header
  {
    uint8_t address;
    uint8_t checksum;
    uint16_t lastDeviceId;
  };

  static const uint16_t TIMEOUT = 20 * SystemTime::MS;
  static const uint16_t CHECK_PERFORMANCE_TIMEOUT = 2 * SystemTime::S;
  static const uint8_t MAX_RETRIES = 3;
  static const uint8_t MAX_BUS_TIMES = 8;

  enum State
  {
    IDLE,
    SLAVE_BUSY,
    MASTER_BUSY
  };

  class Command
  {
  public:

    enum Commands
    {
      GET_CONFIGURATION = HACF::COMMANDS_START,
      SET_CONFIGURATION,
      CHECK_BUS_TIMING,
      GET_BUS_TIMING,
      RESET_BUS_TIMING,
      GET_CONNECTED_DEVICES,
      CHECK_BUS_PERFORMANCE,

      CORRUPT_NEXT_MSG = 16,
    };

    union Parameter
    {
      uint8_t setConfiguration;
      uint8_t bitNumber;
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
      BUS_TIMING,
      CONNECTED_DEVICES,
      BUS_PERFORMANCE,
    };

    struct BusTiming
    {
      uint16_t sclTimes[MAX_BUS_TIMES];
      uint16_t sdaTimes[MAX_BUS_TIMES];
    };

    struct ConnectedDevices
    {
      uint16_t deviceIds[MAX_CONNECTED_DEVICES];
    };

    struct BusPerformance
    {
      uint16_t duration;
      uint16_t failedDeviceIds[MAX_CONNECTED_DEVICES];
    };

    union Parameter
    {
      uint8_t configuration;
      BusTiming busTiming;
      ConnectedDevices connectedDevices;
      BusPerformance busPerformance;
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

    void setConfiguration( uint8_t value );

    void setBusTiming( BusTiming& timing );

    void setConnectedDevices( DeviceArray* deviceArray );

    void setBusPerformance( uint16_t duration, DeviceArray* deviceArray );

    ////    Attributes    ////

  private:

    Parameter params;
  };

  ////    Constructors and destructors    ////

  inline TwiStream( uint8_t twiPort, uint8_t _id ) :
    retries( MAX_RETRIES ), busyBusRetries( MAX_RETRIES ),
      twi( Twi::instance( twiPort ) ), state( IDLE ), sclPin( twiPort, 1 ),
      sdaPin( twiPort, 0 )
  {
    setId( (ClassId::GATEWAY << 8) | _id );
  }

  ////    Operations    ////

#ifdef _DEBUG_
  uint8_t getTraceInfo();
#endif

  void handleMasterStatus();

  void handleSlaveStatus();

  inline void init();

  bool isBusIdle();

  bool isMyTimeSlot();

  bool notifyEvent( const Event& event );

  void run();

private:

  void notifyEndOfMasterTransfer();

  void notifyEndOfSlaveTransfer();

  bool handleRequest( HACF* message );

  void checkBusTiming( bool active = false );

  void triggerBusLine( DigitalOutput& busLine, uint16_t* times );

  void measureBusLine( DigitalOutput& busLine, uint16_t* times );

  void sendPerformanceResults();

public:

  Stream::TransferDescriptor* getMasterDescriptor() const;

  DigitalOutput* getSclPin() const;

  DigitalOutput* getSdaPin() const;

  Stream::TransferDescriptor* getSlaveDescriptor() const;

protected:

  inline static const uint8_t getDebugLevel()
  {
    return debugLevel;
  }

private:

  void resetBusTiming();

  inline uint8_t getRetries() const
  {
    return retries;
  }

  inline void setRetries( uint8_t p_retries )
  {
    retries = p_retries;
  }

  inline Twi& getTwi() const
  {
    return twi;
  }

  inline void setTwi( Twi& p_twi )
  {
    twi = p_twi;
  }

  inline Timestamp* getLastIdleTimestamp() const
  {
    return (Timestamp*) &lastIdleTimestamp;
  }

  inline uint8_t getState() const
  {
    return state;
  }

  inline void setState( uint8_t p_state )
  {
    state = p_state;
  }

  inline Timestamp* getTimeout() const
  {
    return (Timestamp*) &timeout;
  }

  ////    Attributes    ////

protected:

  static const uint8_t debugLevel;

private:

  uint8_t retries;

  uint8_t busyBusRetries;

  Twi& twi;

  Timestamp lastIdleTimestamp;

  Timestamp checkPerformanceTimestamp;

  uint8_t state;

  Timestamp timeout;

//  Timestamp checkBusTimingCmd;

  Response::BusTiming busTiming;

  ////    Relations and components    ////

protected:

  Stream::TransferDescriptor masterDescriptor;

  DigitalOutput sclPin;

  DigitalOutput sdaPin;

  Stream::TransferDescriptor slaveDescriptor;
};

inline void TwiStream::init()
{
  twi.init<true, 100000, TWI_MASTER_INTLVL_OFF_gc, TWI_SLAVE_INTLVL_OFF_gc>();
  lastIdleTimestamp = Timestamp();
  if ( (numOfGateways > 1) && (deviceArray == 0) )
  {
    deviceArray = new DeviceArray();
    sentPingIds = new DeviceArray();
  }
}

#endif
