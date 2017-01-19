/*
 * I2c.cpp
 *
 *  Created on: 21.08.2015
 *      Author: viktor.pankraz
 */

#include <HwProtocols/I2c.h>

const uint8_t I2c::debugLevel( DEBUG_LEVEL_OFF );

I2c::I2c( PortPin sclPin, PortPin sdaPin ) :
    state( IDLE ), scl( sclPin.getPortNumber(), sclPin.getPinNumber() ),
    sda( sdaPin.getPortNumber(), sdaPin.getPinNumber() )
{
  scl.getIoPort().configure( scl.getPin(), PORT_OPC_WIREDANDPULL_gc, false,
                             PORT_ISC_BOTHEDGES_gc, true );
  scl.set();
  scl.configOutput();

  sda.getIoPort().configure( sda.getPin(), PORT_OPC_WIREDANDPULL_gc, false,
                             PORT_ISC_BOTHEDGES_gc, true );
  sda.set();
  sda.configOutput();
}

bool I2c::isIdle()
{
  return ((state == IDLE) && scl.isSet() && sda.isSet());
}

void I2c::start()
{
  if ( isIdle() )
  {
    sda.clear();
    if ( scl.isSet() )
    {
      state = MASTER;
    }
    else
    {
      state = SLAVE;
    }
  }
}

void I2c::stop()
{
  sda.set();
  state = IDLE;
  while ( !isIdle() )
    ;
}

uint16_t I2c::sendReceive( uint8_t* data, uint16_t sendLength,
                           uint16_t receiveLength )
{
  uint16_t len = 0;
  if ( sendLength && isIdle() )
  {
    start();
    while ( isMaster() && (len < sendLength) )
    {
      sendReceiveByte( data[len++] );
    }
    if ( isMaster() )
    {
      stop();
    }
  }

  while ( isSlave() && (len < receiveLength) )
  {
    sendReceiveByte( data[len++] );
  }

  return len;
}

void I2c::sendReceiveByte( uint8_t& data )
{
//  uint8_t* sdaReg = ((uint8_t*) &sda.getIoPort()) + 8;
//  uint8_t sdaPin = sda.getPin();
  uint8_t* sclReg = ((uint8_t*) &scl.getIoPort()) + 8;
  uint8_t sclPin = scl.getPin();

  for ( uint8_t j = 0; j < 8; j++ )
  {
//    uint8_t bit = data & 0x80;
    if ( isMaster() )
    {
      scl.clear();
    }

    uint16_t remainingTime = delayBit( COUNT_DELAY_BIT_US( 20 ), sclReg, sclPin,
                                       0 );
    if ( !remainingTime )
    {
      stop();
      return;
    }
  }
}
