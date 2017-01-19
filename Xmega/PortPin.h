/*
 * PortPin.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Basics_PortPin_H
#define Basics_PortPin_H

#include "Basics.h"
#include "Peripherals/IoPort.h"

class PortPin
{

public:

  enum States
  {
    LOW,
    HIGH
  };

  ////    Constructors and destructors    ////

  PortPin( uint8_t _portNumber, uint8_t _pinNumber );

  ////    Operations    ////

  void configInput();

  void configOutput();

  inline IoPort& getIoPort() const
  {
    return IoPort::instance( portNumber );
  }

  uint8_t getPin() const;

  inline uint8_t isInverted();

  inline void setInverted( bool inverted );

  ////    Additional operations    ////

  inline uint8_t getPinNumber() const
  {
    return pinNumber;
  }

  inline void setPinNumber( uint8_t p_pinNumber )
  {
    pinNumber = p_pinNumber;
  }

  inline uint8_t getPortNumber() const
  {
    return portNumber;
  }

  inline void setPortNumber( uint8_t p_portNumber )
  {
    portNumber = p_portNumber;
  }

  ////    Attributes    ////

  uint8_t pinNumber :4;	

  uint8_t portNumber :4;		
};

inline uint8_t PortPin::isInverted()
{
  return getIoPort().isPinInverted( pinNumber );
}

inline void PortPin::setInverted( bool inverted )
{
  getIoPort().setPinInverted( pinNumber, inverted );
}

#endif
