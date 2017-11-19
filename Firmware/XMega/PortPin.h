/*
 * PortPin.h
 *
 *  Created on: 17.07.2017
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

      inline bool isValid() const
      {
         return portNumber < PortMax;
      }

      uint8_t getPin() const;

      inline uint8_t isInverted();

      inline void enablePullup()
      {
         getIoPort().setPinMode( pinNumber, PORT_OPC_PULLUP_gc );
      }

      inline void enablePulldown()
      {
         getIoPort().setPinMode( pinNumber, PORT_OPC_PULLDOWN_gc );
      }

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

      uint8_t pinNumber : 4;

      uint8_t portNumber : 4;
};

inline uint8_t PortPin::isInverted()
{
   return getIoPort().isPinInverted( pinNumber );
}

inline void PortPin::setInverted( bool inverted )
{
   getIoPort().setPinInverted( pinNumber, inverted );
}


template<uint8_t portNumber, uint8_t pinNumber>
class PortPinTmpl
{

   public:

      enum States
      {
         LOW,
         HIGH
      };

      ////    Constructors and destructors    ////

      inline PortPinTmpl()
      {
      }

      ////    Operations    ////

      inline void configInput()
      {
         getIoPort().setPinsAsInput( getPin() );
      }

      inline void configOutput()
      {
         getIoPort().setPinsAsOutput( getPin() );
      }

      inline IoPort& getIoPort() const
      {
         return IoPort::instance<portNumber>();
      }

      inline uint8_t getPin() const
      {
         return ( 1 << pinNumber );
      }

      inline bool isValid() const
      {
         return portNumber < PortMax;
      }

      inline uint8_t isInverted()
      {
         return getIoPort().isPinInverted( pinNumber );
      }

      inline void setInverted( bool inverted )
      {
         getIoPort().setPinInverted( pinNumber, inverted );
      }

      inline void enablePullup()
      {
         getIoPort().setPinMode( pinNumber, PORT_OPC_PULLUP_gc );
      }

      inline void enablePulldown()
      {
         getIoPort().setPinMode( pinNumber, PORT_OPC_PULLDOWN_gc );
      }

      ////    Additional operations    ////

      inline uint8_t getPinNumber() const
      {
         return pinNumber;
      }

      inline uint8_t getPortNumber() const
      {
         return portNumber;
      }
};

#endif
