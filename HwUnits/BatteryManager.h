/*
 * BatteryManager.h
 *
 *  Created on: 11.04.2016
 *      Author: viktor.pankraz
 */

#ifndef BATTERYMANAGER_H_
#define BATTERYMANAGER_H_

#include <HwUnits.h>
#include <BaseSensorUnit.h>
#include <DigitalOutput.h>
#include <AnalogInput.h>

class BatteryManager: public BaseSensorUnit
{
  enum SubStates
  {
    NO_ERROR,
    POWER_SUPPLY_NOT_CONNECTED,
    POWER_SUPPLY_NOK,
    BATTERY_NOT_CONNECTED,
    BATTERY_REVERSE_POLARITY,
  };

  class Command
  {
  public:

    enum Commands
    {
      ERASE = HACF::COMMANDS_START,
      GET_LOGS,
    };

    union Parameter
    {
      uint32_t sector;
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
      LOG_DATA = HACF::RESULTS_START,
    };

    union Parameter
    {
      uint8_t data;
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

    Parameter& setLogData();

    ////    Attributes    ////

  private:

    Parameter params;
  };

public:

  ////    Constructors and destructors    ////

  BatteryManager( DigitalOutput _charger, DigitalInput _powerSupply,
                  AnalogInput _batteryGndVoltage );

  ////    Operations    ////

  virtual bool notifyEvent( const Event& event );

  void run();

  uint8_t getBatteryConnectionStatus();

  inline bool isPowerSupplyOk() const
  {
    return ( powerSupply.isSet() == 0 );
  }

  inline SubStates getCurrentSubState()
  {
    return (SubStates)state.sub;
  }

protected:

  bool hasBatteryStatusChanged();

  SubStates getBatteryStatus();

  inline void disableCharger()
  {
    charger.set();
  }

  inline void enableCharger()
  {
    charger.clear();
  }

private:

  void notifyCurrentBatteryValue();

  void sendErrorStatus();

  ////    Attributes    ////

private:

  static const uint8_t debugLevel;

  uint16_t batteryValue;

  DigitalOutput charger;
  DigitalInput powerSupply;
  AnalogInput batteryGndVoltage;

};

#endif /* BATTERYMANAGER_H_ */
