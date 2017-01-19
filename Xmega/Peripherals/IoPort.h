/*********************************************************************
 Rhapsody	: 8.0.3
 Login		: viktor.pankraz
 Component	: Xmega192A3
 Configuration 	: debug
 Model Element	: IoPort
 //!	Generated Date	: Tue, 24, Jun 2014
 File Path	: Xmega192A3/debug/Peripherals/IoPort.h
 *********************************************************************/

#ifndef Peripherals_IoPort_H
#define Peripherals_IoPort_H

//## auto_generated
#include "Peripherals/Peripherals.h"
//## package Peripherals

//## class IoPort
class IoPort
{
public:

  ////    Constructors and destructors    ////

protected:

  //## operation IoPort()
  inline IoPort()
  {
    //#[ operation IoPort()
    //#]
  }

  ////    Operations    ////

public:

  //## operation clearInterruptFlags(uint8_t)
  inline void clearInterruptFlags( uint8_t bitMask )
  {
    //#[ operation clearInterruptFlags(uint8_t)
    reg.INTFLAGS = bitMask;
    //#]
  }

  //## operation clearPins(uint8_t)
  inline void clearPins( uint8_t pins )
  {
    //#[ operation clearPins(uint8_t)
    reg.OUTCLR = pins;
    //#]
  }

  //## operation configure(uint8_t,PORT_OPC_t,bool,PORT_ISC_t,bool)
  inline void configure( uint8_t pins, PORT_OPC_t opc,
                         bool invertEnable = false, PORT_ISC_t isc =
                             PORT_ISC_BOTHEDGES_gc,
                         bool slewRateEnable = false )
  {
    //#[ operation configure(uint8_t,PORT_OPC_t,bool,PORT_ISC_t,bool)
    // Build pin control register value.
    uint8_t temp = (uint8_t) opc | isc | (slewRateEnable ? PORT_SRLEN_bm : 0)
        | (invertEnable ? PORT_INVEN_bm : 0);

    // Configure the pins in one atomic operation.
    {
      CriticalSection doNotInterrupt;

      PORTCFG.MPCMASK = pins;
      reg.PIN0CTRL = temp;
    }
    //#]
  }

  //## operation disableInterrupt0()
  inline void disableInterrupt0()
  {
    //#[ operation disableInterrupt0()
    reg.INTCTRL &= ~PORT_INT0LVL_gm;
    //#]
  }

  //## operation disableInterrupt0Source(uint8_t)
  inline void disableInterrupt0Source( uint8_t pins )
  {
    //#[ operation disableInterrupt0Source(uint8_t)
    reg.INT0MASK &= ~pins;
    //#]
  }

  //## operation disableInterrupt1()
  inline void disableInterrupt1()
  {
    //#[ operation disableInterrupt1()
    reg.INTCTRL &= ~PORT_INT1LVL_gm;
    //#]
  }

  //## operation disableInterrupt1Source(uint8_t)
  inline void disableInterrupt1Source( uint8_t pins )
  {
    //#[ operation disableInterrupt1Source(uint8_t)
    reg.INT1MASK &= ~pins;
    //#]
  }

  //## operation enableInterrupt0(PORT_INT0LVL_t)
  inline void enableInterrupt0( PORT_INT0LVL_t level = PORT_INT0LVL_LO_gc )
  {
    //#[ operation enableInterrupt0(PORT_INT0LVL_t)
    reg.INTCTRL = (reg.INTCTRL & ~PORT_INT0LVL_gm) | level;
    //#]
  }

  //## operation enableInterrupt0Source(uint8_t)
  inline void enableInterrupt0Source( uint8_t pins )
  {
    //#[ operation enableInterrupt0Source(uint8_t)
    reg.INT0MASK |= pins;
    //#]
  }

  //## operation enableInterrupt1(PORT_INT1LVL_t)
  inline void enableInterrupt1( PORT_INT1LVL_t level = PORT_INT1LVL_LO_gc )
  {
    //#[ operation enableInterrupt1(PORT_INT1LVL_t)
    reg.INTCTRL = (reg.INTCTRL & ~PORT_INT1LVL_gm) | level;
    //#]
  }

  //## operation enableInterrupt1Source(uint8_t)
  inline void enableInterrupt1Source( uint8_t pins )
  {
    //#[ operation enableInterrupt1Source(uint8_t)
    reg.INT1MASK |= pins;
    //#]
  }

  //## operation get()
  inline uint8_t get() const
  {
    //#[ operation get()
    return reg.IN;
    //#]
  }

  //## operation getInputPins()
  inline uint8_t getInputPins()
  {
    //#[ operation getInputPins()
    return ~getOutputPins();
    //#]
  }

  //## operation getInterruptFlags(uint8_t)
  inline uint8_t getInterruptFlags( uint8_t bitMask )
  {
    //#[ operation getInterruptFlags(uint8_t)
    return reg.INTFLAGS & bitMask;
    //#]
  }

  //## operation getOutputPins()
  inline uint8_t getOutputPins()
  {
    //#[ operation getOutputPins()
    return reg.DIR;
    //#]
  }

  //## operation instance(uint8_t)
  static IoPort& instance( uint8_t portNumber );

  //## operation isPinInverted(uint8_t)
  inline uint8_t isPinInverted( uint8_t pinNumber )
  {
    //#[ operation isPinInverted(uint8_t)
    if ( pinNumber < 8 )
    {
      return *(&reg.PIN0CTRL + pinNumber) & PORT_INVEN_bm;
    }
    return 0;
    //#]
  }

  //## operation isPinSet(uint8_t)
  inline uint8_t isPinSet( uint8_t pin )
  {
    //#[ operation isPinSet(uint8_t)
    return get() & pin;
    //#]
  }

  //## operation set(uint8_t)
  inline void set( uint8_t value )
  {
    //#[ operation set(uint8_t)
    reg.OUT = value;
    //#]
  }

  //## operation setDirection(uint8_t)
  inline void setDirection( uint8_t pins )
  {
    //#[ operation setDirection(uint8_t)
    reg.DIR = pins;
    //#]
  }

  //## operation setPinInverted(uint8_t,bool)
  inline void setPinInverted( uint8_t pinNumber, bool inverted )
  {
    //#[ operation setPinInverted(uint8_t,bool)
    if ( pinNumber < 8 )
    {
      if ( inverted )
      {
        *(&reg.PIN0CTRL + pinNumber) |= PORT_INVEN_bm;
      }
      else
      {
        *(&reg.PIN0CTRL + pinNumber) &= ~PORT_INVEN_bm;
      }
    }
    //#]
  }

  //## operation setPins(uint8_t)
  inline void setPins( uint8_t pins )
  {
    //#[ operation setPins(uint8_t)
    reg.OUTSET = pins;
    //#]
  }

  //## operation setPinsAsInput(uint8_t)
  inline void setPinsAsInput( uint8_t pins )
  {
    //#[ operation setPinsAsInput(uint8_t)
    reg.DIRCLR = pins;
    //#]
  }

  //## operation setPinsAsOutput(uint8_t)
  inline void setPinsAsOutput( uint8_t pins )
  {
    //#[ operation setPinsAsOutput(uint8_t)
    reg.DIRSET = pins;
    //#]
  }

  //## operation toggleDirection(uint8_t)
  inline void toggleDirection( uint8_t pins )
  {
    //#[ operation toggleDirection(uint8_t)
    reg.DIRTGL = pins;
    //#]
  }

  //## operation togglePins(uint8_t)
  inline void togglePins( uint8_t pins )
  {
    //#[ operation togglePins(uint8_t)
    reg.OUTTGL = pins;
    //#]
  }

  ////    Attributes    ////

protected:

  PORT_t reg;		//## attribute reg
};

#endif
/*********************************************************************
 File Path	: Xmega192A3/debug/Peripherals/IoPort.h
 *********************************************************************/
