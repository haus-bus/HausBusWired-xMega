/*
 * Dht.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_Dht_H
#define HwUnits_Dht_H

#include "HwUnits.h"
#include "BaseSensorUnit.h"
#include <Dht22.h>
class Event;

class PortPin;

class Scheduler;

class Dht: public BaseSensorUnit
{
public:

  class Temperature;

  struct Data
  {
    uint8_t checksum;
    BaseSensorUnit::Status temperature;
    BaseSensorUnit::Status humidity;
  };

  class Temperature: public BaseSensorUnit
  {
    ////    Constructors and destructors    ////

  public:

    Temperature( uint8_t instanceNumber );

    ////    Operations    ////

    virtual bool notifyEvent( const Event& event );
  };

  ////    Constructors and destructors    ////

  Dht( uint8_t instanceNumber, PortPin portPin );

  ////    Operations    ////

  void handleRunning();

  virtual bool notifyEvent( const Event& event );

  inline void * operator new( size_t size );

  void run();

  ////    Additional operations    ////

  Dht22* getHardware() const;

  Temperature* getItsTemperature() const;

private:

  inline static const uint8_t getDebugLevel()
  {
    return debugLevel;
  }

  ////    Attributes    ////

  static const uint8_t debugLevel;

  ////    Relations and components    ////

protected:

  Dht22 hardware;

  Temperature itsTemperature;
};

inline void * Dht::operator new( size_t size )
{
  return allocOnce( size );
}

#endif
