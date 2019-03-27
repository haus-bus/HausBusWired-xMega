/*
 * Eeprom.cpp
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#include "Eeprom.h"

#include <GlobalInterrupt.h>
#include <string.h>

const uint8_t Eeprom::debugLevel( DEBUG_LEVEL_OFF );


void Eeprom::executeCommand( NVM_CMD_t command )
{
   // #[ operation executeCommand(NVM_CMD_t)
   uint8_t oldCmd = NVM.CMD;

   CriticalSection doNotInterrupt;

   NVM.CMD = command;

   // Save the Sleep register
   uint8_t sleepCtr = SLEEP.CTRL;
   // Set sleep mode to IDLE
   SLEEP.CTRL = ( SLEEP.CTRL & ~SLEEP.CTRL ) | SLEEP_SMODE_IDLE_gc;
   // Save the PMIC Status and control registers
   uint8_t statusStore = PMIC.STATUS;
   uint8_t pmicStore = PMIC.CTRL;

   // Enable only the highest level of interrupts
   PMIC.CTRL = ( PMIC.CTRL & ~( PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm ) ) | PMIC_HILVLEN_bm;

   // Set sleep enabled
   SLEEP.CTRL |= SLEEP_SEN_bm;
   // Save eeprom interrupt settings for later
   uint8_t eepromintStore = NVM.INTCTRL;

   // Write the "safety code" to the CCP regiter
   // EEPROM write has to be executed within 4 cycles
   CCP = CCP_IOREG_gc;
   // Execute command to write buffer page to EEPROM
   NVM.CTRLA = NVM_CMDEX_bm;
   // Enable EEPROM interrupt
   NVM.INTCTRL = NVM_EELVL0_bm | NVM_EELVL1_bm;

   // Sleep before 2.5uS has passed
   GlobalInterrupt::enable();
   GlobalInterrupt::waitForInterrupt();

   // Restore sleep settings
   SLEEP.CTRL = sleepCtr;
   // Restore PMIC status and control registers
   PMIC.STATUS = statusStore;
   PMIC.CTRL = pmicStore;
   // Restore EEPROM interruptsettings
   NVM.INTCTRL = eepromintStore;

   NVM.CMD = oldCmd;
   NvmController::waitWhileBusy();
}

uint8_t Eeprom::read( uint16_t offset )
{
   DEBUG_M2( FSTR( "ee.r " ), offset );
   NvmController::waitWhileBusy();
   if ( MemoryMapped::isEnabled() )
   {
      if ( offset < MAPPED_EEPROM_START )
      {
         offset += MAPPED_EEPROM_START;
      }
      return *( (uint8_t*)offset );
   }

   MemoryMapped map;
   uint8_t* ptr = (uint8_t*)MAPPED_EEPROM_START;
   uint8_t value = ptr[offset];
// barrier();

   return value;
}

uint16_t Eeprom::read( uint16_t offset, void* pData, uint16_t length )
{
   DEBUG_M4( FSTR( "ee.r " ), offset, ' ', length );
   NvmController::waitWhileBusy();
   if ( MemoryMapped::isEnabled() )
   {
      if ( offset < MAPPED_EEPROM_START )
      {
         offset += MAPPED_EEPROM_START;
      }
      return MemoryMapped::read( offset, pData, length );
   }
   {
      MemoryMapped map;
      memcpy( pData, (void*)( MAPPED_EEPROM_START + offset ), length );
   }
   return length;
}

bool Eeprom::write( uint16_t offset, uint8_t value )
{
   DEBUG_M2( FSTR( "ee.w " ), offset );
   if ( MemoryMapped::isEnabled() )
   {
      if ( offset < MAPPED_EEPROM_START )
      {
         offset += MAPPED_EEPROM_START;
      }
      return MemoryMapped::write( offset, &value, 1 );
   }

   if ( !isAddressInRange( offset ) )
   {
      return false;
   }
   flushBuffer();

   NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

   // Set address to write to
   NVM.ADDR2 = 0x00;
   NVM.ADDR1 = ( offset >> 8 ) & 0xFF;
   NVM.ADDR0 = offset & 0xFF;

   // Load data to write, which triggers the loading of EEPROM page buffer
   NVM.DATA0 = value;
   executeCommand( NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc );

   return true;
}

uint16_t Eeprom::write( uint16_t offset, const void* pData, uint16_t length )
{
   if ( MemoryMapped::isEnabled() )
   {
      if ( offset < MAPPED_EEPROM_START )
      {
         offset += MAPPED_EEPROM_START;
      }
      return MemoryMapped::write( offset, pData, length );
   }
   const uint8_t* source = static_cast<const uint8_t*>( pData );
   while ( length )
   {
      if ( !write( offset++, *source++ ) )
      {
         break;
      }
      length--;
   }
   return length;
}

void Eeprom::fillBufferWithValue( uint8_t value )
{
   uint8_t oldCmd = NVM.CMD;

   flushBuffer();

   NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

   /*  Set address to zero, as only the lower bits matters. ADDR0 is
    *  maintained inside the loop below.
    */
   NVM.ADDR2 = 0x00;
   NVM.ADDR1 = 0x00;

   // Load multiple bytes into page buffer
   for ( uint8_t i = 0; i < EEPROM_PAGE_SIZE; ++i )
   {
      NVM.ADDR0 = i;
      NVM.DATA0 = value;
   }
   NVM.CMD = oldCmd;
   NvmController::waitWhileBusy();
}

bool Eeprom::isAddressInRange( uint16_t address )
{
   if ( ( address < EEPROM_START )
      || ( address > EEPROM_END ) )
   {
      ERROR_2( address, " isOutOfRange" );
      return false;
   }
   return true;
}

bool Eeprom::MemoryMapped::isAddressInRange( uint16_t address )
{
   if ( ( address < MAPPED_EEPROM_START )
      || ( address > MAPPED_EEPROM_END ) )
   {
      ERROR_2( address, " isOutOfRange" );
      return false;
   }
   return true;
}

uint16_t Eeprom::MemoryMapped::write( uint16_t offset, const void* pData, uint16_t length )
{
   if ( !isAddressInRange( offset ) || !isAddressInRange( offset + length ) )
   {
      return 0;
   }

   uint16_t writtenLength = 0;
   const uint8_t* source = static_cast<const uint8_t*>( pData );
   while ( length )
   {
      uint8_t pageLength = EEPROM_PAGE_SIZE - ( offset % EEPROM_PAGE_SIZE );
      if ( pageLength > length )
      {
         pageLength = length;
      }
      Eeprom::flushBuffer();
      memcpy( (void*)( offset ), source, pageLength );

      // Set address to write to
      NVM.ADDR2 = 0x00;
      NVM.ADDR1 = ( offset >> 8 ) & 0xFF;
      NVM.ADDR0 = offset & 0xFF;

      Eeprom::executeCommand( NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc );

      source += pageLength;
      length -= pageLength;
      writtenLength += pageLength;
      offset += pageLength;
   }
   return writtenLength;
}

uint16_t Eeprom::MemoryMapped::read( uint16_t offset, void* pData, uint16_t length )
{
   if ( !isAddressInRange( offset ) || !isAddressInRange( offset + length ) )
   {
      return 0;
   }
   NvmController::waitWhileBusy();
   memcpy( pData, (void*)( offset ), length );

   return length;
}

SIGNAL(NVM_EE_vect)
{
   // Disable the EEPROM interrupt
   NVM.INTCTRL = ( NVM.INTCTRL & ~NVM_EELVL_gm );
}
