/*
 * Eeprom.h
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Eeprom_H
#define Peripherals_Eeprom_H

#include "NvmController.h"

#include <SwFramework.h>

class GlobalInterrupt;

class Eeprom
{
   public:

      class MemoryMapped
      {
         ////    Constructors and destructors    ////

         public:

            inline MemoryMapped()
            {
               NVM.CTRLB |= NVM_EEMAPEN_bm;
            }

            inline ~MemoryMapped()
            {
               NVM.CTRLB &= ~NVM_EEMAPEN_bm;
            }

            static inline void enable()
            {
               NvmController::waitWhileBusy(); NVM.CTRLB |= NVM_EEMAPEN_bm;
            }

            static inline void disable()
            {
               NvmController::waitWhileBusy(); NVM.CTRLB &= ~NVM_EEMAPEN_bm;
            }

            static inline bool isEnabled()
            {
               return NVM.CTRLB & NVM_EEMAPEN_bm;
            }

            static bool isAddressInRange( uint16_t address );

            static uint16_t write( uint16_t offset, const void* pData, uint16_t length );

            static uint16_t read( uint16_t offset, void* pData, uint16_t length );
      };

      ////    Operations    ////

      inline static void disablePowerReduction()
      {
         NVM.CTRLB &= ~NVM_EPRM_bm;
      }

      inline static void enablePowerReduction()
      {
         NVM.CTRLB |= NVM_EPRM_bm;
      }

      inline static void erase();

      static void executeCommand( NVM_CMD_t command );

      static uint8_t read( uint16_t offset );

      static uint16_t read( uint16_t offset, void* pData, uint16_t length );

      static bool write( uint16_t offset, uint8_t value );

      static uint16_t write( uint16_t offset, const void* pData, uint16_t length );

   protected:

      static void fillBufferWithValue( uint8_t value );

      inline static void flushBuffer();

   private:

      static bool isAddressInRange( uint16_t address );

      ////    Attributes    ////

   protected:

      static const uint8_t debugLevel;
};

inline void Eeprom::erase()
{
   WARN_1( "ee.erase" );
   bool mapped = MemoryMapped::isEnabled();
   MemoryMapped::disable();
   fillBufferWithValue( 0xFF );
   NvmController::waitWhileBusy();
   executeCommand( NVM_CMD_ERASE_EEPROM_gc );
   if ( mapped )
   {
      MemoryMapped::enable();
   }
}

inline void Eeprom::flushBuffer()
{
   NvmController::waitWhileBusy();

   // Flush EEPROM page buffer if necessary.
   if ( ( NVM.STATUS & NVM_EELOAD_bm ) != 0 )
   {
      DEBUG_M1( FSTR( "flush" ) );
      executeCommand( NVM_CMD_ERASE_EEPROM_BUFFER_gc );
   }
}

#endif

