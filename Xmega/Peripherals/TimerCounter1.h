/*
 * TimerCounter1.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_TimerCounter1_H
#define Peripherals_TimerCounter1_H

#include "Peripherals/Peripherals.h"
#include "Peripherals/TimerCounterBase.h"

class TimerCounter1 : public TimerCounterBase
{
   public:

      typedef struct TC1_struct
      {
         register8_t reserved_0x2C;
         register8_t reserved_0x2D;
         register8_t reserved_0x2E;
         register8_t reserved_0x2F;
         register8_t reserved_0x30;
         register8_t reserved_0x31;
         register8_t reserved_0x32;
         register8_t reserved_0x33;
         register8_t reserved_0x34;
         register8_t reserved_0x35;
         _WORDREGISTER( PERBUF ); /* Period Buffer */
         _WORDREGISTER( CCABUF ); /* Compare Or Capture A Buffer */
         _WORDREGISTER( CCBBUF ); /* Compare Or Capture B Buffer */
      } TC1_t;

      ////    Constructors and destructors    ////

   protected:

      inline TimerCounter1()
      {
      }

      ////    Operations    ////

   public:

      inline void disableChannels( uint8_t disableMask )
      {
         // Make sure only CCxEN bits are set in disableMask.
         disableMask &= ( TC1_CCAEN_bm | TC1_CCBEN_bm );

         // Disable channels.
         baseReg.CTRLB &= ~disableMask;
      }

      inline void enableChannels( uint8_t enableMask )
      {
         // Make sure only CCxEN bits are set in enableMask.
         enableMask &= ( TC1_CCAEN_bm | TC1_CCBEN_bm );

         // Enable channels.
         baseReg.CTRLB |= enableMask;
      }

      static TimerCounter1& instance( uint8_t portNumber );

      inline void setCompareA( uint16_t value )
      {
         reg.CCABUF = value;
      }

      inline void setCompareB( uint16_t value )
      {
         reg.CCBBUF = value;
      }

      inline void setPeriodBuffered( uint16_t value )
      {
         reg.PERBUF = value;
      }

      ////    Attributes    ////

   protected:

      TC1_t reg;
};

#endif
