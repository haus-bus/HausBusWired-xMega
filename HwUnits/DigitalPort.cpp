/*
 * DigitalPort.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "DigitalPort.h"

#ifdef USE_DHT           
#include "Dht.h"
#endif

#ifdef USE_OW                     
#include <OneWire.h>
#include "DS1820.h"
#endif

#ifdef USE_IR          
#include "IrReceiver.h"
#endif

#ifdef USE_ANALOGIN
#include "AnalogInputUnit.h"
#endif

#ifdef USE_MDIGIN
#include "MonitoredDigitalInput.h"
#endif


const uint8_t DigitalPort::debugLevel( DEBUG_LEVEL_OFF );

DigitalPort::Response::Parameter& DigitalPort::Response::setConfiguration()
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().configuration) );
  setResponse( CONFIGURATION );
  return getParameter();
}

bool DigitalPort::notifyEvent( const Event& event )
{
  if ( event.isEvWakeup() )
  {
    run();
  }
  else if ( event.isEvMessage() )
  {
    return handleRequest( event.isEvMessage()->getMessage() );
  }

  return false;
}

void DigitalPort::run()
{
  if ( inStartUp() )
  {
    configureHw();
    setMainState( RUNNING );
  }
  else
  {
    updateLeds();

    uint8_t changedPins = debounce();
    notifyButtonChanges( changedPins );
    setSleepTime( HwConfiguration::DigitalPort::RefreshTime );
  }

}

void DigitalPort::updateLeds()
{
  uint8_t i = HwConfiguration::DigitalPort::MAX_PINS;
  uint8_t pos = 0x80;
  while ( i-- )
  {
    Led* led = pin[i].led;
    if ( led && led->isClassId( ClassId::LED ) )
    {
      hardware.on( pos, led->getHwBrightness() );
    }
    pos >>= 1;
  }
}

bool DigitalPort::handleRequest( HACF* message )
{
  bool consumed = true;
  HACF::ControlFrame& cf = message->controlFrame;
  Command* data = reinterpret_cast<Command*>( cf.getData() );
  if ( cf.isCommand( Command::GET_CONFIGURATION ) )
  {
    DEBUG_H1( FSTR(".getConfiguration()") );
    Response response( getId(), *message );
    configuration->get( response.setConfiguration().configuration );
    response.queue( getObject( message->header.getSenderId() ) );
  }
  else if ( cf.isCommand( Command::SET_CONFIGURATION ) )
  {
    DEBUG_H1( FSTR(".setConfiguration()") );
    configuration->set( data->parameter.setConfiguration );
  }
  else
  {
    return false;
  }

  return consumed;
}

void DigitalPort::clearPinFunction()
{
  uint8_t i = HwConfiguration::DigitalPort::MAX_PINS;
  while ( i-- )
  {
    pin[i].button = 0;
  }
}

void DigitalPort::configureHw()
{
  configuration = HwConfiguration::getDigitalPortConfiguration( id );

  uint8_t outputMask = 0;
  uint8_t inputMask = 0;
  uint8_t pwmMask = 0;
  uint8_t subId = getId() & 0xF0;
  uint8_t portNumber = (subId >> 4) - 1;

  uint8_t i = HwConfiguration::DigitalPort::MAX_PINS;
  uint8_t bit = 0x80;
  while ( i-- )
  {
    bool notSupported = false;
    uint8_t pinFunction = configuration->getPinFunction( i );
    if ( pinFunction != 0xFF )
    {
      if ( isPinUsable( bit ) )
      {
        if ( pinFunction == ClassId::BUTTON )
        {
          pin[i].button = new Button( subId + i + 1 );
          inputMask |= bit;
        }
        else if ( pinFunction == ClassId::COUNTER )
        {
          pin[i].counter = new Counter( subId + i + 1 );
          inputMask |= bit;
        }
        else if ( pinFunction == ClassId::LED )
        {
          pin[i].led = new Led( PortPin( portNumber, i ) );
          pwmMask |= bit;
        }
        else if ( pinFunction == ClassId::DIGITAL_OUTPUT )
        {
          new PortPinUnit( PortPin( portNumber, i ) );
          outputMask |= bit;
        }
#ifdef USE_ANALOGIN
        else if ( pinFunction == ClassId::ANALOG_INPUT )
        {
          // analog inputs has to be high impedance
          new AnalogInputUnit( PortPin( portNumber, i ) );
        }
#endif
#ifdef USE_MDIGIN
        else if ( pinFunction == ClassId::MONITORED_DIGITAL_INPUT )
        {
          // monitored inputs have to be high impedance
          new MonitoredDigitalInput( PortPin( portNumber, i ) );
        }
#endif
#ifdef USE_DHT
        else if ( pinFunction == ClassId::HUMIDITY )
        {
          new Dht( subId + i + 1, PortPin( portNumber, i ) );
          inputMask |= bit;
        }
#endif
#ifdef USE_IR
        else if ( pinFunction == ClassId::IR_RECEIVER )
        {
          new IrReceiver( subId + i + 1, PortPin( portNumber, i ) );
          inputMask |= bit;
        }
#endif
#ifdef USE_OW
        else if ( pinFunction == ClassId::TEMPERATURE )
        {
          OneWire ow( PortPin( portNumber, i ) );
          // wait 1ms to stabilize the line
          SystemTime::waitMs(1);
          DEBUG_M4( FSTR("1-Wire port: "), portNumber, FSTR("pin: "), i );

          // check for 1-Wire components
          uint8_t diff = OneWire::SEARCH_FIRST;
          OneWire::RomCode romCode;

          while ( diff != OneWire::LAST_DEVICE )
          {
            diff = ow.searchROM( diff, (uint8_t*) &romCode );
            if ( diff == OneWire::PRESENCE_ERROR )
            {
              DEBUG_M1( FSTR("No devices found") );
              break;
            }
            else if ( diff == OneWire::DATA_ERROR )
            {
              DEBUG_M1( FSTR("Bus Error") );
              break;
            }
            else
            {
              DEBUG_M1( FSTR("0x") );
              for ( uint8_t i = 0; i < OneWire::ROMCODE_SIZE; i++ )
              {
                DEBUG_L1( ((uint8_t* )&romCode)[i] );
              }

              if ( DS1820::isSensor( romCode.family ) )
              {
                DEBUG_L1( FSTR("->DS18X20") );
                new DS1820( ow, romCode );
              }
              else
              {
                DEBUG_L1( FSTR("->UNKNOWN") );
              }
            }
          }
          inputMask |= bit;
        }
#endif
        else
        {
          notSupported = true;
        }
      }
      else
      {
        notSupported = true;
      }
    }
    if ( notSupported )
    {
      Response event( getId() );
      event.setErrorCode( PINFUNCTION_NOT_SUPPORTED + i );
      event.queue();
      configuration->setPinFunction( i, 0xFF );
    }
    bit >>= 1;
  }

  hardware.configure( outputMask, inputMask, pwmMask );
  if ( !outputMask && !inputMask && !pwmMask )
  {
    setSleepTime( NO_WAKE_UP );
  }
}

uint8_t DigitalPort::debounce()
{
  IoPort& port = hardware.getPort();
  uint8_t i = state ^ ~port.isPinSet( port.getInputPins() );    // key changed ?
  counter0 = ~(counter0 & i);                              // reset or count ct0
  counter1 = counter0 ^ (counter1 & i);                    // reset or count ct1
  i &= counter0 & counter1;                           // count until roll over ?
  state ^= i;                                     // then toggle debounced state

  return i;
}

void DigitalPort::notifyButtonChanges( uint8_t changedPins )
{
  uint8_t i = HwConfiguration::DigitalPort::MAX_PINS;
  uint8_t pos = 0x80;
  while ( i-- )
  {
    if ( ( changedPins & pos ) && pin[i].object )
    {
      if( pin[i].button->isClassId( ClassId::BUTTON ) )
      {
        pin[i].button->updateState( state & pos );
      }
      else if( pin[i].counter->isClassId( ClassId::COUNTER ) )
      {
        pin[i].counter->updateState( state & pos );
      }
    }
    pos >>= 1;
  }
}

HwConfiguration::DigitalPort* DigitalPort::getConfiguration() const
{
  return configuration;
}

void DigitalPort::setConfiguration(
    HwConfiguration::DigitalPort* p_DigitalPort )
{
  configuration = p_DigitalPort;
}
