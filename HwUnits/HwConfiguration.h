/*
 * HwConfiguration.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_HwConfiguration_H
#define HwUnits_HwConfiguration_H

#include "HwUnits.h"

#include <SwConfiguration.h>
#include <Peripherals/Flash.h>

class Checksum;

class Scheduler;

class HwConfiguration: public SwConfiguration
{
public:

  class Button;

  class DaliLine;

  class DigitalPort;

  class Dimmer;

  class Ethernet;

  class LogicalButton;

  class PortPin;

  class RollerShutter;

  class SensorUnit;

  class HomeAutomation;

  class UdpStream;

  class SnmpAgent
  {
  public:

    ////    Operations    ////

    inline void restoreDefaults();

    inline void get( SnmpAgent& configuration ) const;

    inline uint32_t getTrapDestinationIp() const;

    inline void getCommunityString( char* string ) const;

    inline void set( SnmpAgent& configuration );

    ////    Attributes    ////

  private:

    uint32_t trapDestinationIp;

    char communityString[17];

  };

  class Dimmer
  {
  public:

    static const uint8_t TIME_FACTOR = 50;

    enum Mode
    {
      DIMM_CR, // CR
      DIMM_L, // L
      SWITCH,
      MAX_MODE
    };

    static const uint8_t DEFAULT_MODE = DIMM_CR;
    static const uint8_t DEFAULT_FADING_TIME = 600 / TIME_FACTOR;
    static const uint8_t DEFAULT_DIMMING_TIME = 3000 / TIME_FACTOR;
    static const uint8_t DEFAULT_DIMMING_RANGE_START = 0;
    static const uint8_t DEFAULT_DIMMING_RANGE_END = 100;

    static const uint8_t MAX_FADING_TIME = 200;
    static const uint8_t MAX_DIMMING_TIME = 200;
    static const uint8_t MAX_DIMMING_RANGE_START = 100;
    static const uint8_t MAX_DIMMING_RANGE_END = 100;

    ////    Operations    ////

    void get( Dimmer& configuration ) const;

    inline uint8_t getDimmingRangeEnd();

    inline uint8_t getDimmingRangeStart();

    inline uint8_t getDimmingTime();

    inline uint8_t getFadingTime();

    inline uint8_t getMode();

    inline void restoreDefaults();

    void set( Dimmer& configuration );

    ////    Attributes    ////

  private:

    uint8_t mode;

    uint8_t fadingTime;

    uint8_t dimmingTime;

    uint8_t dimmingRangeStart;

    uint8_t dimmingRangeEnd;

  };

  class RollerShutter
  {
  public:

    static const uint16_t TIME_FACTOR = 1000;

    static const uint8_t DEFAULT_OPTIONS = 0;
    static const uint8_t DEFAULT_CLOSE_TIME = 20;
    static const uint8_t DEFAULT_OPEN_TIME = 21;
    static const uint8_t MAX_LEVEL = 100;
    static const uint16_t DEFAULT_MOTOR_START_DELAY = 256;

    static const uint8_t OFFSET_POSITION = 4;

    ////    Operations    ////

    void get( RollerShutter& configuration ) const;

    inline uint8_t getCloseTime() const;

    inline uint8_t getOpenTime() const;

    inline uint8_t getPosition() const;

    inline uint8_t isInverted() const;

    inline void restoreDefaults();

    void set( RollerShutter& configuration );

    inline void setPosition( uint8_t position );

    ////    Attributes    ////

  private:

    uint8_t closeTime;

    uint8_t openTime;

    uint8_t inverted :1;

  };

  class Button
  {
  public:

    static const uint8_t DEFAULT_HOLD_TIMEOUT = 100;
    static const uint8_t DEFAULT_DOUBLE_CLICK_TIMEOUT = 50;
    static const uint8_t DEFAULT_EVENTS = 0xA1;

    struct EventBits
    {
      uint8_t notifyOnCovered :1;
      uint8_t notifyOnClicked :1;
      uint8_t notifyOnDoubleClicked :1;
      uint8_t notifyOnHoldStart :1;
      uint8_t notifyOnHoldEnd :1;
      uint8_t notifyOnFree :1;
      uint8_t reserved :1;
      uint8_t feedbackEnabled :1;
    };

    union Event
    {
      uint8_t mask;
      EventBits bit;
    };

    ////    Operations    ////

    void get( Button& configuration ) const;

    inline uint8_t getDoubleClickTimeout() const;

    inline Event getEvents() const;

    inline uint8_t getHoldTimeout() const;

    inline void restoreDefaults();

    void set( Button& configuration );

    ////    Attributes    ////

  private:

    uint8_t holdTimeout;

    uint8_t doubleClickTimeout;

    Event events;

  };

  class Counter
  {
  public:

    struct Mode
    {
      uint8_t increment :1;
      uint8_t falling :1;
      uint8_t rising :1;
    };
    enum EMode
    {
      DECREMENT_FALLING = 0x02,
      INCREMENT_FALLING = 0x03,
      DEREMENT_RISING = 0x04,
      INCREMENT_RISING = 0x05,
      DEREMENT_EITHER = 0x06,
      INCREMENT_EITHER = 0x07,
    };

    static const uint8_t DEFAULT_MODE = INCREMENT_FALLING;
    static const uint16_t DEFAULT_REPORT_TIME = 60;
    static const uint32_t DEFAULT_THRESHOLD = 0;
    static const uint8_t OFFSET_VALUE = 17;

    ////    Operations    ////

    void get( Counter& configuration ) const;

    inline Mode getMode() const;

    inline uint16_t getReportTime() const;

    inline uint32_t getThreshold() const;

    inline uint32_t getValue() const;

    inline void restoreDefaults();

    void set( Counter& configuration );

    inline void setValue( uint32_t value );

    ////    Attributes    ////

  private:

    Mode mode;

    uint16_t reportTime;

    uint32_t threshold;

  };

  class MonitoredDigitalInput
  {
  public:

    static const uint16_t DEFAULT_LOWER_THRESHOLD = 100;
    static const uint16_t DEFAULT_UPPER_THRESHOLD = 200;

    ////    Operations    ////

    inline void restoreDefaults();

    void get( MonitoredDigitalInput& configuration ) const;

    void set( MonitoredDigitalInput& configuration );

    inline uint16_t getLowerThreshold() const;

    inline uint16_t getUpperThreshold() const;

    ////    Attributes    ////

  private:

    uint16_t lowerThreshold;

    uint16_t upperThreshold;

  };

  class PortPin
  {
  public:

    static const uint8_t DEFAULT_DUTY_OFFSET = 0;
    static const uint8_t DEFAULT_MIN_DUTY = 0;
    static const uint8_t DEFAULT_OPTIONS = 0x06; // driveOff, driveOn, !invert
    static const uint16_t DEFAULT_TIME_BASE = 1000;

    struct Options
    {
      uint8_t invert :1;
      uint8_t driveOn :1;
      uint8_t driveOff :1;
      uint8_t reserved :5;
    };

    ////    Operations    ////

    void get( PortPin& configuration ) const;

    inline uint8_t getDutyOffset() const;

    inline uint8_t getMinDuty() const;

    inline Options getOptions() const;

    uint16_t getTimeBase() const;

    inline void restoreDefaults();

    inline void set( PortPin& configuration );

    ////    Attributes    ////

  private:

    uint8_t dutyOffset;

    uint8_t minDuty;

    uint16_t timeBase;

    Options options;

  };

  class DigitalPort
  {
  public:

    static const uint8_t MAX_PINS = 8;

    static const uint8_t RefreshTime = 10;

    ////    Operations    ////

    void get( DigitalPort& configuration ) const;

    uint8_t getPinFunction( uint8_t idx ) const;

    inline void restoreDefaults();

    void set( DigitalPort& configuration );

    void setPinFunction( uint8_t idx, uint8_t value );

    ////    Attributes    ////

  private:

    uint8_t pinFunction[MAX_PINS];

  };

  class LogicalButton
  {
  public:

    static const uint8_t MAX_OBJECTS = 8;

    ////    Operations    ////

    void get( LogicalButton& configuration ) const;

    uint8_t getButtonId( uint8_t index );

    uint8_t getLedId( uint8_t index );

    inline void restoreDefaults();

    inline void set( LogicalButton& configuration );

    ////    Attributes    ////

  private:

    uint8_t buttonId[MAX_OBJECTS];

    uint8_t ledId[MAX_OBJECTS];

  };

  class DaliLine
  {
  public:

    static const uint8_t MAX_DEVICES = 20;

    ////    Operations    ////

    void get( DaliLine& configuration ) const;

    inline void restoreDefaults();

    void set( DaliLine& configuration );

    ////    Attributes    ////

  private:

    uint8_t address[MAX_DEVICES];

  };

  class SensorUnit
  {
  public:

    static const uint8_t DEFAULT_LOWER_THRESOLD = 18;
    static const uint8_t DEFAULT_UPPER_THRESOLD = 22;
    static const uint8_t DEFAULT_REPORT_TIME = 60;
    static const uint8_t DEFAULT_HYSTERESIS = 5;

    ////    Operations    ////

    void get( SensorUnit& configuration ) const;

    inline int8_t getLowerThreshold() const;

    inline uint8_t getReportTime() const;

    inline int8_t getUpperThreshold() const;

    inline uint8_t getHysteresis() const;

    inline void restoreDefaults();

    void set( SensorUnit& configuration );

    ////    Attributes    ////

  private:

    int8_t lowerThreshold;

    int8_t upperThreshold;

    uint8_t reportTime;

    uint8_t hysteresis;
  };

  class UdpStream
  {
  public:

    ////    Operations    ////

    void get( UdpStream& configuration ) const;

    inline void restoreDefaults();

    void set( UdpStream& configuration );

    ////    Attributes    ////

  private:

    uint32_t dummy;
  };

  class RS485Stream
  {
  public:

    ////    Operations    ////

    void get( RS485Stream& configuration ) const;

    inline void restoreDefaults();

    void set( RS485Stream& configuration );

    ////    Attributes    ////

  private:

    uint32_t dummy;
  };

  class Ethernet
  {
  public:

    class Options;

    class Options
    {
      ////    Attributes    ////

    public:

      bool udpPort9Only :1;

      bool modBusTcp :1;

      bool dhcp :1;

      bool snmpAgent :1;
    };

    ////    Operations    ////

    void get( Ethernet& configuration ) const;

    inline uint32_t getIp() const;

    inline Options getOptions() const;

    inline void restoreDefaults();

    void set( Ethernet& configuration );

    ////    Attributes    ////

  private:

    uint32_t ip;

    Options options;
  };

  ////    Operations    ////

  class HomeAutomation
  {
  public:

    static const uint16_t USER_SIGNATURE_ROW_OFFSET = 2;

    static const uint8_t SIZEOF = 13;
    static const uint8_t MAX_SLOTS = 8;
    static const uint8_t MAX_REPORT_TIME = 60;

    ////    Operations    ////

    uint8_t get( HomeAutomation& configuration );

    uint16_t getDeviceId();

    inline uint8_t getLogicalButtonMask() const;

    inline uint8_t getReportMemoryStatusTime();

    inline uint8_t getSlotType( uint8_t idx ) const;

    inline uint8_t getStartupDelay() const;

    inline uint8_t getTimeCorrectionValue() const;

    inline static HomeAutomation& instance();

    inline static void restoreFactoryConfiguration( uint8_t id, uint8_t fcke );

    void set( HomeAutomation& configuration );

    inline void setDeviceId( uint16_t _deviceId );

    inline void setTimeCorrectionValue( uint8_t value );

    ////    Attributes    ////

  private:

    uint8_t startupDelay;

    uint8_t logicalButtonMask;

    uint16_t deviceId;

    uint8_t reportMemoryStatusTime;

    uint8_t slotTypes[MAX_SLOTS];

    uint8_t timeCorrectionValue;

    uint8_t reserve;

    uint8_t checksum;
  };

  inline static SnmpAgent* getSnmpAgentConfiguration( uint16_t id );

  inline static Button* getButtonConfiguration( uint16_t id );

  inline static Counter* getCounterConfiguration( uint16_t id );

  inline static MonitoredDigitalInput* getMonitoredDigitalInputConfiguration( uint16_t id );

  inline static DaliLine* getDaliLineConfiguration( uint16_t id );

  inline static DigitalPort* getDigitalPortConfiguration( uint16_t id );

  inline static Dimmer* getDimmerConfiguration( uint16_t id );

  inline static Ethernet* getEthernetConfiguration( uint16_t id );

  inline static LogicalButton* getLogicalButtonConfiguration( uint16_t id );

  inline static PortPin* getPortPinConfiguration( uint16_t id );

  inline static RollerShutter* getRollerShutterConfiguration( uint16_t id );

  inline static SensorUnit* getSensorUnitConfiguration( uint16_t id );

  inline static UdpStream* getUdpStreamConfiguration( uint16_t id );

  inline static RS485Stream* getRS485StreamConfiguration( uint16_t id );

private:

public:

private:

};

inline HwConfiguration::SnmpAgent* HwConfiguration::getSnmpAgentConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(LargeConfiguration) );
  SnmpAgent* conf =
      (SnmpAgent*) &largeConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::Button* HwConfiguration::getButtonConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(SmallConfiguration) );
  Button* conf =
      (Button*) &smallConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::Counter* HwConfiguration::getCounterConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(LargeConfiguration) );
  Counter* conf =
      (Counter*) &largeConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::MonitoredDigitalInput* HwConfiguration::getMonitoredDigitalInputConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(SmallConfiguration) );
  MonitoredDigitalInput* conf =
      (MonitoredDigitalInput*) &smallConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::DaliLine* HwConfiguration::getDaliLineConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(LargeConfiguration) );
  DaliLine* conf =
      (DaliLine*) &largeConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::DigitalPort* HwConfiguration::getDigitalPortConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(LargeConfiguration) );
  DigitalPort* conf =
      (DigitalPort*) &largeConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::Dimmer* HwConfiguration::getDimmerConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(SmallConfiguration) );
  Dimmer* conf =
      (Dimmer*) &smallConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::Ethernet* HwConfiguration::getEthernetConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(SmallConfiguration) );
  Ethernet* conf =
      (Ethernet*) &smallConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::LogicalButton* HwConfiguration::getLogicalButtonConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(LargeConfiguration) );
  LogicalButton* conf = (LogicalButton*) &largeConfigurations[idx
      & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::PortPin* HwConfiguration::getPortPinConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(SmallConfiguration) );
  PortPin* conf =
      (PortPin*) &smallConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( (idx & NEW_INITIALIZED) || conf->getOptions().reserved )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::RollerShutter* HwConfiguration::getRollerShutterConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(SmallConfiguration) );
  RollerShutter* conf = (RollerShutter*) &smallConfigurations[idx
      & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::SensorUnit* HwConfiguration::getSensorUnitConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(SmallConfiguration) );
  SensorUnit* conf =
      (SensorUnit*) &smallConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::UdpStream* HwConfiguration::getUdpStreamConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(SmallConfiguration) );
  UdpStream* conf =
      (UdpStream*) &smallConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline HwConfiguration::RS485Stream* HwConfiguration::getRS485StreamConfiguration(
    uint16_t id )
{
  uint8_t idx = getIndexOfConfiguration( id, sizeof(SmallConfiguration) );
  RS485Stream* conf =
      (RS485Stream*) &smallConfigurations[idx & ~NEW_INITIALIZED].configData;

  if ( idx & NEW_INITIALIZED )
  {
    conf->restoreDefaults();
  }

  return conf;
}

inline void HwConfiguration::SnmpAgent::restoreDefaults()
{
  uint8_t defaultConfiguration[] = { "\xFF\xFF\xFF\xFFpublic" };
  set( *reinterpret_cast<SnmpAgent*>( defaultConfiguration ) );
}

inline void HwConfiguration::SnmpAgent::get(
    HwConfiguration::SnmpAgent& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

inline uint32_t HwConfiguration::SnmpAgent::getTrapDestinationIp() const
{
  SnmpAgent conf;
  get( conf );
  return conf.trapDestinationIp;
}

void HwConfiguration::SnmpAgent::getCommunityString( char* string ) const
{
  SnmpAgent conf;
  get( conf );
  memcpy( string, conf.communityString, sizeof(conf.communityString) );
}

inline void HwConfiguration::SnmpAgent::set(
    HwConfiguration::SnmpAgent& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

inline uint8_t HwConfiguration::Dimmer::getDimmingRangeEnd()
{
  Dimmer conf;
  get( conf );
  return conf.dimmingRangeEnd;
}

inline uint8_t HwConfiguration::Dimmer::getDimmingRangeStart()
{
  Dimmer conf;
  get( conf );
  return conf.dimmingRangeStart;
}

inline uint8_t HwConfiguration::Dimmer::getDimmingTime()
{
  Dimmer conf;
  get( conf );
  return conf.dimmingTime;
}

inline uint8_t HwConfiguration::Dimmer::getFadingTime()
{
  Dimmer conf;
  get( conf );
  return conf.fadingTime;
}

inline uint8_t HwConfiguration::Dimmer::getMode()
{
  Dimmer conf;
  get( conf );
  return conf.mode;
}

inline void HwConfiguration::Dimmer::restoreDefaults()
{
  uint8_t defaultConfiguration[] = { DEFAULT_MODE, DEFAULT_FADING_TIME,
      DEFAULT_DIMMING_TIME, DEFAULT_DIMMING_RANGE_START,
      DEFAULT_DIMMING_RANGE_END };
  set( *reinterpret_cast<Dimmer*>( defaultConfiguration ) );
}

inline uint8_t HwConfiguration::RollerShutter::getCloseTime() const
{
  RollerShutter conf;
  get( conf );
  return conf.closeTime;
}

inline uint8_t HwConfiguration::RollerShutter::getOpenTime() const
{
  RollerShutter conf;
  get( conf );
  return conf.openTime;
}

inline uint8_t HwConfiguration::RollerShutter::getPosition() const
{
  uint8_t position;
  storage->read( reinterpret_cast<uint16_t>( this ) + OFFSET_POSITION,
                 &position, sizeof(position) );

  if ( position > MAX_LEVEL )
  {
    position = MAX_LEVEL;
  }
  return position;
}

inline uint8_t HwConfiguration::RollerShutter::isInverted() const
{
  RollerShutter conf;
  get( conf );
  return conf.inverted;
}

inline void HwConfiguration::RollerShutter::restoreDefaults()
{
  uint8_t defaultConfiguration[] = { DEFAULT_CLOSE_TIME, DEFAULT_OPEN_TIME,
      DEFAULT_OPTIONS };
  set( *reinterpret_cast<RollerShutter*>( defaultConfiguration ) );
}

inline void HwConfiguration::RollerShutter::setPosition( uint8_t position )
{
  storage->write( reinterpret_cast<uint16_t>( this ) + OFFSET_POSITION,
                  &position, sizeof(position) );
}

inline uint8_t HwConfiguration::Button::getDoubleClickTimeout() const
{
  Button conf;
  get( conf );
  return conf.doubleClickTimeout;
}

inline HwConfiguration::Button::Event HwConfiguration::Button::getEvents() const
{
  Button conf;
  get( conf );
  return conf.events;
}

inline uint8_t HwConfiguration::Button::getHoldTimeout() const
{
  Button conf;
  get( conf );
  return conf.holdTimeout;
}

inline void HwConfiguration::Button::restoreDefaults()
{
  uint8_t defaultConfiguration[] = { DEFAULT_HOLD_TIMEOUT,
      DEFAULT_DOUBLE_CLICK_TIMEOUT, DEFAULT_EVENTS };
  set( *reinterpret_cast<Button*>( defaultConfiguration ) );
}

inline HwConfiguration::Counter::Mode HwConfiguration::Counter::getMode() const
{
  Counter conf;
  get( conf );
  return conf.mode;
}

inline uint16_t HwConfiguration::Counter::getReportTime() const
{
  Counter conf;
  get( conf );
  return conf.reportTime;
}

inline uint32_t HwConfiguration::Counter::getThreshold() const
{
  Counter conf;
  get( conf );
  return conf.threshold;
}

inline uint32_t HwConfiguration::Counter::getValue() const
{
  uint32_t value;
  storage->read( reinterpret_cast<uint16_t>( this ) + OFFSET_VALUE, &value,
                 sizeof(value) );

  return value;
}

inline void HwConfiguration::Counter::setValue( uint32_t value )
{
  storage->write( reinterpret_cast<uint16_t>( this ) + OFFSET_VALUE, &value,
                  sizeof(value) );
}

inline void HwConfiguration::Counter::restoreDefaults()
{
  uint8_t defaultConfiguration[] = { DEFAULT_MODE, LBYTE( DEFAULT_REPORT_TIME ),
      HBYTE( DEFAULT_REPORT_TIME ), LBYTE( LWORD(DEFAULT_THRESHOLD) ), HBYTE(
          LWORD(DEFAULT_THRESHOLD) ), LBYTE( HWORD(DEFAULT_THRESHOLD) ), HBYTE(
          HWORD(DEFAULT_THRESHOLD) ) };
  set( *reinterpret_cast<Counter*>( defaultConfiguration ) );
}

inline uint16_t HwConfiguration::MonitoredDigitalInput::getLowerThreshold() const
{
  MonitoredDigitalInput conf;
  get( conf );
  return conf.lowerThreshold;
}

inline uint16_t HwConfiguration::MonitoredDigitalInput::getUpperThreshold() const
{
  MonitoredDigitalInput conf;
  get( conf );
  return conf.upperThreshold;
}

inline void HwConfiguration::MonitoredDigitalInput::restoreDefaults()
{
  uint8_t defaultConfiguration[] = { LBYTE( DEFAULT_LOWER_THRESHOLD ),
      HBYTE( DEFAULT_LOWER_THRESHOLD ), LBYTE( DEFAULT_UPPER_THRESHOLD ),
      HBYTE( DEFAULT_UPPER_THRESHOLD ) };
  set( *reinterpret_cast<MonitoredDigitalInput*>( defaultConfiguration ) );
}

inline uint8_t HwConfiguration::PortPin::getDutyOffset() const
{
  PortPin conf;
  get( conf );
  return conf.dutyOffset;
}

inline uint8_t HwConfiguration::PortPin::getMinDuty() const
{
  PortPin conf;
  get( conf );
  return conf.minDuty;
}

inline HwConfiguration::PortPin::Options HwConfiguration::PortPin::getOptions() const
{
  PortPin conf;
  get( conf );
  return conf.options;
}

inline void HwConfiguration::PortPin::restoreDefaults()
{
  uint8_t defaultConfiguration[] = { DEFAULT_DUTY_OFFSET, DEFAULT_MIN_DUTY,
      LBYTE( DEFAULT_TIME_BASE ), HBYTE( DEFAULT_TIME_BASE ), DEFAULT_OPTIONS };
  set( *reinterpret_cast<PortPin*>( defaultConfiguration ) );
}

inline void HwConfiguration::PortPin::set(
    HwConfiguration::PortPin& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

inline void HwConfiguration::DigitalPort::restoreDefaults()
{
}

inline void HwConfiguration::LogicalButton::restoreDefaults()
{
}

inline void HwConfiguration::LogicalButton::set(
    HwConfiguration::LogicalButton& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

inline void HwConfiguration::DaliLine::restoreDefaults()
{
  uint8_t defaultConfiguration[MAX_DEVICES];
  memset( defaultConfiguration, 0, sizeof(defaultConfiguration) );
  set( *reinterpret_cast<DaliLine*>( defaultConfiguration ) );
}

inline int8_t HwConfiguration::SensorUnit::getLowerThreshold() const
{
  SensorUnit conf;
  get( conf );
  return conf.lowerThreshold;
}

inline uint8_t HwConfiguration::SensorUnit::getReportTime() const
{
  SensorUnit conf;
  get( conf );
  return conf.reportTime;
}

inline int8_t HwConfiguration::SensorUnit::getUpperThreshold() const
{
  SensorUnit conf;
  get( conf );
  return conf.upperThreshold;
}

inline uint8_t HwConfiguration::SensorUnit::getHysteresis() const
{
  SensorUnit conf;
  get( conf );
  return conf.hysteresis;
}

inline void HwConfiguration::SensorUnit::restoreDefaults()
{
  uint8_t defaultConfiguration[] = { DEFAULT_LOWER_THRESOLD,
      DEFAULT_UPPER_THRESOLD, DEFAULT_REPORT_TIME, DEFAULT_HYSTERESIS };
  set( *reinterpret_cast<SensorUnit*>( defaultConfiguration ) );
}

inline void HwConfiguration::UdpStream::restoreDefaults()
{
  //uint32_t defaultConfiguration[] = { CONST_IP( 255, 255, 255, 255 ) };
  //set( *reinterpret_cast<UdpStream*>(&defaultConfiguration) );
}

inline void HwConfiguration::RS485Stream::restoreDefaults()
{
  //uint32_t defaultConfiguration[] = { CONST_IP( 255, 255, 255, 255 ) };
  //set( *reinterpret_cast<UdpStream*>(&defaultConfiguration) );
}

inline uint32_t HwConfiguration::Ethernet::getIp() const
{
  Ethernet conf;
  get( conf );
  return conf.ip;
}

inline HwConfiguration::Ethernet::Options HwConfiguration::Ethernet::getOptions() const
{
  Ethernet conf;
  get( conf );
  return conf.options;
}

inline void HwConfiguration::Ethernet::restoreDefaults()
{
  uint8_t defaultConfiguration[] = { 192, 168, 0, 254, 1 };
  set( *reinterpret_cast<Ethernet*>( &defaultConfiguration ) );
}

inline uint8_t HwConfiguration::HomeAutomation::getLogicalButtonMask() const
{
  return Flash::readUserSignature(
      reinterpret_cast<uint16_t>( &logicalButtonMask ) );
}

inline uint8_t HwConfiguration::HomeAutomation::getReportMemoryStatusTime()
{
  uint8_t value = Flash::readUserSignature(
      reinterpret_cast<uint16_t>( &reportMemoryStatusTime ) );
  ;

  if ( value > MAX_REPORT_TIME )
  {
    return MAX_REPORT_TIME;
  }
  return value;
}

inline uint8_t HwConfiguration::HomeAutomation::getSlotType( uint8_t idx ) const
{
  return Flash::readUserSignature(
      reinterpret_cast<uint16_t>( &slotTypes[idx] ) );
}

inline uint8_t HwConfiguration::HomeAutomation::getStartupDelay() const
{
  return Flash::readUserSignature( reinterpret_cast<uint16_t>( &startupDelay ) );
}

inline uint8_t HwConfiguration::HomeAutomation::getTimeCorrectionValue() const
{
  return Flash::readUserSignature(
      reinterpret_cast<uint16_t>( &timeCorrectionValue ) );
}

inline void HwConfiguration::HomeAutomation::setTimeCorrectionValue(
    uint8_t value )
{
  Flash::writeUserSignature( reinterpret_cast<uint16_t>( &timeCorrectionValue ),
                             &value, sizeof(value) );
}

inline HwConfiguration::HomeAutomation& HwConfiguration::HomeAutomation::instance()
{
  return *reinterpret_cast<HomeAutomation*>( USER_SIGNATURE_ROW_OFFSET );
}

inline void HwConfiguration::HomeAutomation::restoreFactoryConfiguration(
    uint8_t id, uint8_t fcke )
{
  uint8_t buffer[sizeof(HomeAutomation) + 2];
  memset( &buffer, 0xFF, sizeof(buffer) );
  buffer[0] = id;
  buffer[1] = fcke;
  buffer[2] = 0;
  buffer[3] = 0;
  buffer[4] = 0xFF;
  buffer[5] = 0x7F;
  Flash::writeUserSignature( 0, &buffer, sizeof(buffer) );
}

inline void HwConfiguration::HomeAutomation::setDeviceId( uint16_t _deviceId )
{
  HomeAutomation conf;
  get( conf );
  conf.deviceId = _deviceId;
  set( conf );
}

#endif
