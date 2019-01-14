/*
 * IoPort.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_IoPort_H
#define Peripherals_IoPort_H

#include "Peripherals.h"

class IoPort
{
   public:

      ////    Constructors and destructors    ////

   protected:

      inline IoPort()
      {
         // only for PortDummy
         reg.IN = 0xFF;
      }

      ////    Operations    ////

   public:

      inline void clearInterruptFlags( uint8_t bitMask )
      {
         reg.INTFLAGS = bitMask;
      }

      inline void clearPins( uint8_t pins )
      {
         reg.OUTCLR = pins;
      }

      inline void configure( uint8_t pins, PORT_OPC_t opc, bool invertEnable = false,
                             PORT_ISC_t isc = PORT_ISC_BOTHEDGES_gc, bool slewRateEnable = false )
      {
         // Build pin control register value.
         uint8_t temp = (uint8_t) opc | isc | ( slewRateEnable ? PORT_SRLEN_bm : 0 )
                        | ( invertEnable ? PORT_INVEN_bm : 0 );

         // Configure the pins in one atomic operation.
         {
            CriticalSection doNotInterrupt;

            PORTCFG.MPCMASK = pins;
            reg.PIN0CTRL = temp;
         }
      }

      inline void disableInterrupt0()
      {
         reg.INTCTRL &= ~PORT_INT0LVL_gm;
      }

      inline void disableInterrupt0Source( uint8_t pins )
      {
         reg.INT0MASK &= ~pins;
      }

      inline void disableInterrupt1()
      {
         reg.INTCTRL &= ~PORT_INT1LVL_gm;
      }

      inline void disableInterrupt1Source( uint8_t pins )
      {
         reg.INT1MASK &= ~pins;
      }

      inline void enableInterrupt0( PORT_INT0LVL_t level = PORT_INT0LVL_LO_gc )
      {
         reg.INTCTRL = ( reg.INTCTRL & ~PORT_INT0LVL_gm ) | level;
      }

      inline void enableInterrupt0Source( uint8_t pins )
      {
         reg.INT0MASK |= pins;
      }

      inline uint8_t isInterrupt0SourceEnabled( uint8_t pins )
      {
         return reg.INT0MASK & pins;
      }

      inline void enableInterrupt1( PORT_INT1LVL_t level = PORT_INT1LVL_LO_gc )
      {
         reg.INTCTRL = ( reg.INTCTRL & ~PORT_INT1LVL_gm ) | level;
      }

      inline void enableInterrupt1Source( uint8_t pins )
      {
         reg.INT1MASK |= pins;
      }

      inline uint8_t isInterrupt1SourceEnabled( uint8_t pins )
      {
         return reg.INT1MASK & pins;
      }

      inline uint8_t get() const
      {
         return reg.IN;
      }

      inline uint8_t getInputPins()
      {
         return ~getOutputPins();
      }

      inline uint8_t getInterruptFlags( uint8_t bitMask )
      {
         return reg.INTFLAGS & bitMask;
      }

      inline uint8_t getOutputPins()
      {
         return reg.DIR;
      }

      static IoPort& instance( uint8_t portNumber );

      template<uint8_t portNumber>
      static inline IoPort& instance()
      {

         switch ( portNumber )
         {
        #ifdef PORTA
            case PortA:
               return *reinterpret_cast<IoPort*>( &PORTA );
        #endif

        #ifdef PORTB
            case PortB:
               return *reinterpret_cast<IoPort*>( &PORTB );
        #endif

        #ifdef PORTC
            case PortC:
               return *reinterpret_cast<IoPort*>( &PORTC );
        #endif

        #ifdef PORTD
            case PortD:
               return *reinterpret_cast<IoPort*>( &PORTD );
        #endif

        #ifdef PORTE
            case PortE:
               return *reinterpret_cast<IoPort*>( &PORTE );
        #endif

        #ifdef PORTF
            case PortF:
               return *reinterpret_cast<IoPort*>( &PORTF );
        #endif

        #ifdef PORTR
            case PortR:
               return *reinterpret_cast<IoPort*>( &PORTR );
        #endif
         }
         return dummyPort;
      }

      inline uint8_t isPinInverted( uint8_t pinNumber )
      {
         if ( pinNumber < 8 )
         {
            return *( &reg.PIN0CTRL + pinNumber ) & PORT_INVEN_bm;
         }
         return 0;
      }

      inline uint8_t isPinSet( uint8_t pin )
      {
         return get() & pin;
      }

      inline void set( uint8_t value )
      {
         reg.OUT = value;
      }

      inline void setDirection( uint8_t pins )
      {
         reg.DIR = pins;
      }

      inline void setPinInverted( uint8_t pinNumber, bool inverted )
      {
         if ( pinNumber < 8 )
         {
            if ( inverted )
            {
               *( &reg.PIN0CTRL + pinNumber ) |= PORT_INVEN_bm;
            }
            else
            {
               *( &reg.PIN0CTRL + pinNumber ) &= ~PORT_INVEN_bm;
            }
         }
      }

      inline void setPinMode( uint8_t pinNumber, PORT_OPC_t opc )
      {
         if ( pinNumber < 8 )
         {
            *( &reg.PIN0CTRL + pinNumber ) &= ~PORT_OPC_WIREDANDPULL_gc;
            *( &reg.PIN0CTRL + pinNumber ) |= opc;
         }
      }

      inline void setPins( uint8_t pins )
      {
         reg.OUTSET = pins;
      }

      inline void setPinsAsInput( uint8_t pins )
      {
         reg.DIRCLR = pins;
      }

      inline void setPinsAsOutput( uint8_t pins )
      {
         reg.DIRSET = pins;
      }

      inline void toggleDirection( uint8_t pins )
      {
         reg.DIRTGL = pins;
      }

      inline void togglePins( uint8_t pins )
      {
         reg.OUTTGL = pins;
      }

      ////    Attributes    ////

   protected:

      static IoPort dummyPort;

      PORT_t reg;
};

#endif
