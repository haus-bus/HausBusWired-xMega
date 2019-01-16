/*
 * ModBusSlave.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "ModBusSlave.h"
#include "Button.h"
#include "Dimmer.h"
#include <Protocols/IpStack/IpConnection.h>
#include "Led.h"
#include "RollerShutter.h"
#include <IStream.h>

const uint8_t ModBusSlave::debugLevel( DEBUG_LEVEL_OFF );

ModBusSlave::Device::Port::Pin ModBusSlave::Device::Port::getPin( int i1 ) const
{
   return pin[i1];
}

void ModBusSlave::Device::Port::setPin(
   int i1, const ModBusSlave::Device::Port::Pin& p_pin )
{
   pin[i1] = p_pin;
}

int ModBusSlave::Device::getPort() const
{
   int iter = 0;
   return iter;
}

bool ModBusSlave::DeviceInfo::isAr8() const
{
   return ar8;
}

void ModBusSlave::DeviceInfo::setAr8( bool p_ar8 )
{
   ar8 = p_ar8;
}

ModBusSlave::ModBusSlave()
{
   setId( ( Object::ClassId::MODBUS_SERVER << 8 ) | 1 );
   memset( deviceInfo, 0, sizeof( deviceInfo ) );
   memset( device, 0, sizeof( device ) );
}

uint8_t ModBusSlave::convertToModBusClassId( uint8_t hacfClassId )
{
   if ( hacfClassId == Object::ClassId::DIGITAL_OUTPUT )
   {
      return DIGITAL_OUT;
   }
   if ( hacfClassId == Object::ClassId::BUTTON )
   {
      return DIGITAL_IN;
   }
   else if ( hacfClassId == Object::ClassId::LED )
   {
      return ANALOG_OUT;
   }
   return 0;
}

bool ModBusSlave::isInput( uint8_t _id )
{
   return ( _id >= DIGITAL_IN );
}

bool ModBusSlave::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      run();
   }
   else if ( event.isEvMessage() )
   {
      DEBUG_H1( FSTR( ".evMessage" ) );

      HACF* message = event.isEvMessage()->getMessage();
      HACF::ControlFrame& cf = message->controlFrame;

      Response response( getId(), *message );

      if ( cf.isCommand( Command::READ_COIL_STATUS ) )
      {
         DEBUG_H1( FSTR( ".readCoilStatus()" ) );
         // Command::ReadCoilStatus rcs = data->parameter.readCoilStatus;
         // if( !readStatus( data->parameter.readCoilStatus.regId, data->parameter.readCoilStatus.len, response.setCoilStatus() ))
      }
      else if ( cf.isCommand( Command::READ_INPUT_STATUS ) )
      {
         DEBUG_H1( FSTR( ".readInputStatus()" ) );
      }
      else if ( cf.isCommand( Command::READ_HOLDING_REGISTERS ) )
      {
         DEBUG_H1( FSTR( ".readHoldingRegisters()" ) );
      }
   }
   else if ( event.isEvGatewayMessage() )
   {
      DEBUG_H1( FSTR( ".evGatewayMessage" ) );

      HACF* message = event.isEvGatewayMessage()->getMessage();
      HACF::ControlFrame& cf = message->controlFrame;
      if ( !cf.isCommand() )
      {

         HACF::Object& sender = cf.senderId;
         if ( ( HACF::deviceId >> 8 ) == sender.getNetworkId() )
         {
            uint8_t _device
               = ( message->isFromThisDevice() ? 0 : sender.getAddress() );
            if ( ( _device < MAX_DEVICES )
               && isClassIdSupported( sender.getClassId() ) )
            {
               uint8_t _port = ( ( sender.getInstanceId() & 0xF0 ) >> 4 ) - 1;
               uint8_t _pin = ( sender.getInstanceId() & 0xF ) - 1;

               // map port pins if necessary
               if ( ( _port == PortE ) || ( _port == PortF ) )
               {
                  // map PortE and PortF always to PortC and PortD
                  _port -= 2;
               }
               if ( _port == PortR )
               {
                  // map PortR Pin0 and Pin1 to PortC Pin4 and Pin5 for IO128
                  _port = PortC;
                  _pin += 4;
               }

               if ( _port < MAX_PORTS )
               {
                  // TODO
// if ( deviceInfo[_device].isAr8() && (_pin > MAX_PINS) )
// {
// _port = PortB;
// _pin -= 8;
// }
                  if ( _pin < MAX_PINS )
                  {
                     Device::Port::Pin& pin = device[_device].port[_port].pin[_pin];
                     pin.value = updateValueFromMsg( *message, pin.value );
                     pin.id = convertToModBusClassId( sender.getClassId() );
                     DEBUG_H4( FSTR( "new value for device : " ), _device,
                               FSTR( " classId : " ), pin.id );
                     DEBUG_M4( FSTR( "port[" ), _port, FSTR( "].pin[" ), _pin );
                     DEBUG_L2( FSTR( "] = " ), pin.value );
                  }
               }
            }
         }
      }
      delete message;
   }
   else
   {
      DEBUG_H1( FSTR( ".ModBusTcp::notifyEvent()" ) );
      return ModBusTcp::notifyEvent( event );
   }
   return false;
}

ModBusTcp::Adu::Exception ModBusSlave::readRegister(
   ModBusTcp::ReadRegister* cmd, bool inputsOnly )
{
   uint16_t length = cmd->getLength();
   uint16_t addr = cmd->getAddress();

   DEBUG_H4( FSTR( "readRegister from 0x" ), addr, FSTR( ", length 0x" ), length );

   if ( !length || ( ( addr + length ) >= ( sizeof( device ) / 2 ) ) )
   {
      return ModBusTcp::Adu::EX_ILLEGAL_DATA_ADDRESS;
   }
   RegisterAddress* regAddr = (RegisterAddress*) &addr;

   Device::Port::Pin* pin
      = &device[regAddr->device].port[regAddr->port].pin[regAddr->pin];

   DEBUG_M4( FSTR( "device[" ), regAddr->device, FSTR( "].port[" ), regAddr->port );
   DEBUG_L3( FSTR( "].pin[" ), regAddr->pin, ']' );

   uint16_t* ptr = cmd->changeIntoResponse();

   while ( length-- )
   {
      uint16_t tmpValue = pin->value;
      DEBUG_M2( FSTR( "value: 0x" ), tmpValue );
      *ptr++ = changeEndianness( tmpValue );
      pin++;
   }
   return ModBusTcp::Adu::EX_NONE;
}

uint8_t ModBusSlave::readStatus( uint16_t registerId, uint8_t length,
                                 uint8_t* data )
{
   DEBUG_H4( FSTR( "readStatus from 0x" ), registerId, FSTR( ", length 0x" ),
             length );
   if ( !length || ( ( registerId + length ) >= ( sizeof( device ) / 2 ) ) )
   {
      return 1;
   }
   RegisterAddress* regAddr = (RegisterAddress*) &registerId;
   Device::Port::Pin* pin
      = &device[regAddr->device].port[regAddr->port].pin[regAddr->pin];

   DEBUG_M4( FSTR( "device[" ), regAddr->device, FSTR( "].port[" ), regAddr->port );
   DEBUG_L3( FSTR( "].pin[" ), regAddr->pin, ']' );

   while ( length )
   {
      *data = 0;
      for ( uint8_t i = 0; i < 8; i++ )
      {
         *data >>= 1;
         if ( length )
         {
            if ( pin->value )
            {
               *data |= 0x80;
            }
            length--;
         }

         pin++;
      }
      DEBUG_M2( FSTR( "value: 0x" ), *data );
      data++;
   }
   return 0;
}

ModBusTcp::Adu::Exception ModBusSlave::readStatus( ModBusTcp::ReadStatus* cmd,
                                                   bool inputsOnly )
{
   uint16_t addr = cmd->getAddress();
   uint16_t length = cmd->getLength();

   if ( readStatus( addr, length, cmd->changeIntoResponse() ) )
   {
      return ModBusTcp::Adu::EX_ILLEGAL_DATA_ADDRESS;
   }
   return ModBusTcp::Adu::EX_NONE;
}

void ModBusSlave::run()
{
   if ( inStartUp() )
   {
      ModBusTcp::notifyEvent( evConnect( this, false ) );
      setSleepTime( NO_WAKE_UP );
   }

   if ( inRunning() )
   {

   }
}

void ModBusSlave::sendCommand( const ModBusSlave::RegisterAddress& regAddr )
{
   static const uint8_t hacfClassId[] = { 0,
                                          // outputs
                                          Object::ClassId::DIGITAL_OUTPUT, Object::ClassId::LED, 0, 0, 0, 0, 0,
                                          // inputs
                                          Object::ClassId::BUTTON, Object::ClassId::TEMPERATURE, };
   Device::Port::Pin& pin
      = device[regAddr.device].port[regAddr.port].pin[regAddr.pin];
   IResponse cmd( getId() );
   cmd.controlFrame.receiverId.setDeviceId( regAddr.device );
   cmd.controlFrame.receiverId.setClassId( hacfClassId[pin.id] );

   uint8_t instanceId;
   // TODO
   // if ( deviceInfo[regAddr.device].isAr8() )
   {
      if ( regAddr.port == PortD )
      {
         // map PortD back to PortF
         instanceId = ( PortF << 4 ) | ( regAddr.pin + 1 );
      }
      else if ( ( regAddr.port == PortC ) && ( regAddr.pin >= 4 ) )
      {
         // map PortC.4/5 back to PortR.0/1
         instanceId = ( PortR << 4 ) | ( regAddr.pin - 4 + 1 );
      }

      instanceId = ( ( regAddr.port + 1 ) << 4 ) | ( regAddr.pin + 1 );
   }
   cmd.controlFrame.receiverId.setInstanceId( instanceId );

   if ( hacfClassId[pin.id] == Object::ClassId::DIGITAL_OUTPUT )
   {
      PortPinUnit::Command* psCmd = (PortPinUnit::Command*) &cmd;
      psCmd->setCommand(
         pin.value ? PortPinUnit::Command::ON : PortPinUnit::Command::OFF );
      cmd.controlFrame.setDataLength( 1 );
      notifyMessageReceived( &cmd.controlFrame );
   }
   else if ( hacfClassId[pin.id] == Object::ClassId::LED )
   {
      Led::Command* ledCmd = (Led::Command*) &cmd;
      ledCmd->setCommand( Led::Command::ON );
      ledCmd->getParameter().on.brightness = pin.value;
      ledCmd->getParameter().on.duration = 0;
      cmd.controlFrame.setDataLength( 3 );
      notifyMessageReceived( &cmd.controlFrame );
   }

}

uint16_t ModBusSlave::updateValueFromMsg( const HACF& event, uint16_t value )
{
   uint8_t classId = event.controlFrame.senderId.getClassId();
   if ( classId == Object::ClassId::BUTTON )
   {
      Button::Response* ev = (Button::Response*) &event;
      value = ( ev->isCovered() ? 1 : 0 );
   }
   else if ( classId == Object::ClassId::LED )
   {
      Led::Response* ev = (Led::Response*) &event;
      switch ( ev->getResponse() )
      {
         case Led::Response::EVENT_OFF:
            value = 0;
            break;
         case Led::Response::EVENT_ON:
         case Led::Response::STATUS:
         {
            value = ev->getParameter().status;
         }
      }
   }
   else if ( classId == Object::ClassId::DIMMER )
   {
      Dimmer::Response* ev = (Dimmer::Response*) &event;
      switch ( ev->getResponse() )
      {
         case Dimmer::Response::EVENT_OFF:
            value = 0;
            break;
         case Dimmer::Response::EVENT_ON:
         case Dimmer::Response::STATUS:
         {
            value = ev->getParameter().status;
         }
      }
   }
   else if ( classId == Object::ClassId::DIGITAL_OUTPUT )
   {
      PortPinUnit::Response* ev = (PortPinUnit::Response*) &event;
      switch ( ev->getResponse() )
      {
         case PortPinUnit::Response::EVENT_OFF:
            value = 0;
            break;
         case PortPinUnit::Response::EVENT_ON:
            value = 0xFFFF;
            break;
         case PortPinUnit::Response::STATUS:
            value = ( ev->getParameter().status ? 1 : 0 );
      }
   }
   else if ( classId == Object::ClassId::ROLLER_SHUTTER )
   {
      RollerShutter::Response* ev = (RollerShutter::Response*) &event;
      switch ( ev->getResponse() )
      {
         case RollerShutter::Response::EVENT_OPEN:
            value = 0;
            break;
         case RollerShutter::Response::EVENT_CLOSED:
         case RollerShutter::Response::STATUS:
         {
            value = ev->getParameter().position;
         }
      }
   }
   else if ( classId == Object::ClassId::TEMPERATURE )
   {
      PortPinUnit::Response* ev = (PortPinUnit::Response*) &event;
      switch ( ev->getResponse() )
      {
         case PortPinUnit::Response::EVENT_OFF:
            value = 0;
            break;
         case PortPinUnit::Response::EVENT_ON:
            value = 0xFFFF;
            break;
         case PortPinUnit::Response::STATUS:
            value = ( ev->getParameter().status ? 1 : 0 );
      }
   }
   return value;
}

ModBusTcp::Adu::Exception ModBusSlave::writeRegister(
   ModBusTcp::WriteRegister* cmd )
{
   uint16_t value = cmd->getValue();
   uint16_t addr = cmd->getAddress();

   DEBUG_H4( FSTR( "writeRegister to 0x" ), addr, FSTR( ", value 0x" ), value );

   if ( addr >= ( sizeof( device ) / 2 ) )
   {
      return ModBusTcp::Adu::EX_ILLEGAL_DATA_ADDRESS;
   }
   RegisterAddress& regAddr = *(RegisterAddress*) &addr;
   Device::Port::Pin& pin
      = device[regAddr.device].port[regAddr.port].pin[regAddr.pin];
   if ( isInput( pin.id ) )
   {
      return ModBusTcp::Adu::EX_ILLEGAL_DATA_VALUE;
   }
   pin.value = value;
   sendCommand( regAddr );
   return ModBusTcp::Adu::EX_NONE;
}

ModBusTcp::Adu::Exception ModBusSlave::writeMultipleRegister(
   ModBusTcp::WriteMultipleRegister* cmd )
{
   uint16_t length = cmd->getLength();
   uint16_t addr = cmd->getAddress();

   DEBUG_H4( FSTR( "writeMultipleRegister to 0x" ), addr, FSTR( ", length 0x" ),
             length );

   if ( !length || ( ( addr + length ) >= ( sizeof( device ) / 2 ) ) )
   {
      return ModBusTcp::Adu::EX_ILLEGAL_DATA_ADDRESS;
   }

   RegisterAddress& regAddr = *(RegisterAddress*) &addr;
   Device::Port::Pin& pin
      = device[regAddr.device].port[regAddr.port].pin[regAddr.pin];
   if ( isInput( pin.id ) )
   {
      return ModBusTcp::Adu::EX_ILLEGAL_DATA_VALUE;
   }

   uint8_t i = 0;
   while ( i < length )
   {
      uint16_t value = cmd->getData( i++ );
      DEBUG_M2( FSTR( "value: 0x" ), value );
      pin.value = value;
      sendCommand( regAddr );
   }
   return ModBusTcp::Adu::EX_NONE;
}

ModBusTcp::Adu::Exception ModBusSlave::writeMultipleCoils(
   ModBusTcp::WriteMultipleCoils* cmd )
{
   uint16_t length = cmd->getLength();
   uint16_t addr = cmd->getAddress();

   DEBUG_H4( FSTR( "writeMultipleCoils to 0x" ), addr, FSTR( ", length 0x" ),
             length );

   if ( !length || ( ( addr + length ) >= ( sizeof( device ) / 2 ) ) )
   {
      return ModBusTcp::Adu::EX_ILLEGAL_DATA_ADDRESS;
   }

   RegisterAddress& regAddr = *(RegisterAddress*) &addr;
   Device::Port::Pin* pin
      = &device[regAddr.device].port[regAddr.port].pin[regAddr.pin];
   if ( isInput( pin->id ) )
   {
      return ModBusTcp::Adu::EX_ILLEGAL_DATA_VALUE;
   }

   const uint8_t* data = cmd->getData();
   while ( length )
   {
      uint8_t value = *data;
      for ( uint8_t i = 0; i < 8; i++ )
      {
         if ( length )
         {
            if ( value & 0x01 )
            {
               pin->value = 0xFFF;
            }
            else
            {
               pin->value = 0;
            }
            DEBUG_M2( FSTR( "value: 0x" ), pin->value );
            sendCommand( regAddr );
            addr++;
            length--;
         }
         value >>= 1;
         pin++;
      }
      data++;
   }
   return ModBusTcp::Adu::EX_NONE;
}

uint8_t ModBusSlave::getDebugLevel()
{
   return debugLevel;
}
