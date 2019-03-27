/*
 * PortPin.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "PortPin.h"

PortPin::PortPin( uint8_t _portNumber, uint8_t _pinNumber ) :
   pinNumber( _pinNumber ), portNumber( _portNumber )
{
}

void PortPin::configInput()
{
   getIoPort().setPinsAsInput( getPin() );
}

void PortPin::configOutput()
{
   getIoPort().setPinsAsOutput( getPin() );
}

uint8_t PortPin::getPin() const
{
   return ( 1 << pinNumber );
}

uint8_t PortPin::isSet() const
{
   return getIoPort().get() & getPin();
}
