/*
 * HwConfiguration.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "HwConfiguration.h"
#include <Security/Checksum.h>

void HwConfiguration::Dimmer::get(
    HwConfiguration::Dimmer& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::Dimmer::set( HwConfiguration::Dimmer& configuration )
{
  if ( configuration.dimmingRangeStart > MAX_DIMMING_RANGE_START )
  {
    configuration.dimmingRangeStart = MAX_DIMMING_RANGE_START;
  }
  if ( configuration.dimmingRangeEnd > MAX_DIMMING_RANGE_END )
  {
    configuration.dimmingRangeEnd = MAX_DIMMING_RANGE_END;
  }
  if ( configuration.dimmingTime > MAX_DIMMING_TIME )
  {
    configuration.dimmingTime = MAX_DIMMING_TIME;
  }
  if ( configuration.fadingTime > MAX_FADING_TIME )
  {
    configuration.fadingTime = MAX_FADING_TIME;
  }
  if ( configuration.dimmingRangeStart >= configuration.dimmingRangeEnd )
  {
    configuration.dimmingRangeStart = DEFAULT_DIMMING_RANGE_START;
    configuration.dimmingRangeEnd = DEFAULT_DIMMING_RANGE_END;
  }
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::RollerShutter::get(
    HwConfiguration::RollerShutter& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::RollerShutter::set(
    HwConfiguration::RollerShutter& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::Button::get(
    HwConfiguration::Button& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::Button::set( HwConfiguration::Button& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::Counter::get(
    HwConfiguration::Counter& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::Counter::set( HwConfiguration::Counter& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::MonitoredDigitalInput::get(
    HwConfiguration::MonitoredDigitalInput& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::MonitoredDigitalInput::set( HwConfiguration::MonitoredDigitalInput& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::PortPin::get(
    HwConfiguration::PortPin& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

uint16_t HwConfiguration::PortPin::getTimeBase() const
{
  PortPin conf;
  get( conf );
  return conf.timeBase;
}

void HwConfiguration::DigitalPort::get(
    HwConfiguration::DigitalPort& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

uint8_t HwConfiguration::DigitalPort::getPinFunction( uint8_t idx ) const
{
  uint8_t function;
  storage->read( reinterpret_cast<uint16_t>( &pinFunction[idx] ), &function,
                 sizeof(function) );
  return function;
}

void HwConfiguration::DigitalPort::set(
    HwConfiguration::DigitalPort& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::DigitalPort::setPinFunction( uint8_t idx, uint8_t value )
{
  storage->write( reinterpret_cast<uint16_t>( &pinFunction[idx] ), &value,
                  sizeof(value) );
}

void HwConfiguration::LogicalButton::get(
    HwConfiguration::LogicalButton& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

uint8_t HwConfiguration::LogicalButton::getButtonId( uint8_t index )
{
  LogicalButton conf;
  get( conf );
  return conf.buttonId[index];
}

uint8_t HwConfiguration::LogicalButton::getLedId( uint8_t index )
{
  LogicalButton conf;
  get( conf );
  return conf.ledId[index];
}

void HwConfiguration::DaliLine::get(
    HwConfiguration::DaliLine& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::DaliLine::set( HwConfiguration::DaliLine& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::SensorUnit::get(
    HwConfiguration::SensorUnit& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::SensorUnit::set(
    HwConfiguration::SensorUnit& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::UdpStream::get(
    HwConfiguration::UdpStream& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::UdpStream::set(
    HwConfiguration::UdpStream& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::RS485Stream::get(
    HwConfiguration::RS485Stream& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::RS485Stream::set(
    HwConfiguration::RS485Stream& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

void HwConfiguration::Ethernet::get(
    HwConfiguration::Ethernet& configuration ) const
{
  storage->read( reinterpret_cast<uint16_t>( this ), &configuration,
                 sizeof(configuration) );
}

void HwConfiguration::Ethernet::set( HwConfiguration::Ethernet& configuration )
{
  storage->write( reinterpret_cast<uint16_t>( this ), &configuration,
                  sizeof(configuration) );
  updateChecksum( reinterpret_cast<uint16_t>( this ), sizeof(configuration) );
}

uint16_t HwConfiguration::HomeAutomation::getDeviceId()
{
  uint16_t id = Flash::readUserSignature(
      reinterpret_cast<uint16_t>( &deviceId ) );
  id |= (Flash::readUserSignature( reinterpret_cast<uint16_t>( &deviceId ) + 1 )
      << 8);
  if ( !id )
  {
    id = 0x7FFF;
  }
  return id;
}

uint8_t HwConfiguration::HomeAutomation::get(
    HwConfiguration::HomeAutomation& configuration )
{

  Flash::readUserSignature( reinterpret_cast<uint16_t>( this ), &configuration,
                            sizeof(configuration) );
  return Checksum::hasError( &configuration, sizeof(configuration) );

  /*

   Configuration confEeprom;
   Eeprom::read( 0, &confEeprom, sizeof(configuration) );

   if ( Checksum::hasError( &configuration, sizeof(configuration) ) )
   {
   errorCode |= 1;
   }
   if ( Checksum::hasError( &confEeprom, sizeof(confEeprom) ) )
   {
   errorCode |= 2;
   }

   if ( errorCode == 1 )
   {
   set( confEeprom );
   configuration = confEeprom;
   }
   if ( memcmp( &configuration, &confEeprom, sizeof(configuration) ) )
   {
   set( configuration );
   }
   */
}

void HwConfiguration::HomeAutomation::set(
    HwConfiguration::HomeAutomation& configuration )
{
  configuration.checksum = 0;
  configuration.checksum = Checksum::get( &configuration,
                                          sizeof(configuration) );

  Flash::writeUserSignature( reinterpret_cast<uint16_t>( this ), &configuration,
                             sizeof(configuration) );
}
