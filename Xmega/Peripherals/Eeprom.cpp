/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: Xmega192A3
        Configuration   : debug
        Model Element	: Eeprom
   //!	Generated Date	: Tue, 24, Jun 2014
        File Path	: Xmega192A3/debug/Peripherals/Eeprom.cpp
 *********************************************************************/

// ## auto_generated
#include "Peripherals/Eeprom.h"
// ## dependency GlobalInterrupt
#include <GlobalInterrupt.h>
// ## package Peripherals

// ## class Eeprom
const uint8_t Eeprom::debugLevel( DEBUG_LEVEL_OFF );

// ## class Eeprom::MemoryMapped

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
   // #]
}

uint8_t Eeprom::read( uint16_t offset )
{
   // #[ operation read(uint16_t)
   DEBUG_M2( FSTR( "ee.r " ), offset );
   NvmController::waitWhileBusy();
   MemoryMapped map;
   uint8_t* ptr = (uint8_t*)MAPPED_EEPROM_START;
   uint8_t value = ptr[offset];
   barrier();

   return value;
   // #]
}

Stream::Status Eeprom::read( uint16_t offset, void* pData, uint16_t length )
{
   // #[ operation read(uint16_t,void *,uint16_t)
   DEBUG_M4( FSTR( "ee.r " ), offset, ' ', length );
   NvmController::waitWhileBusy();

   {
      MemoryMapped map;
      memcpy( pData, (void*)( MAPPED_EEPROM_START + offset ), length );
   }
   return Stream::SUCCESS;
   // #]
}

bool Eeprom::write( uint16_t offset, uint8_t value )
{
   // #[ operation write(uint16_t,uint8_t)
   DEBUG_M2( FSTR( "ee.w " ), offset );
   if ( !isAddressInRange( offset ) )
   {
      return false;
   }

   uint8_t oldCmd = NVM.CMD;
   flushBuffer();
   NvmController::waitWhileBusy();

   NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

   // Set address to write to
   NVM.ADDR2 = 0x00;
   NVM.ADDR1 = ( offset >> 8 ) & 0xFF;
   NVM.ADDR0 = offset & 0xFF;

   // Load data to write, which triggers the loading of EEPROM page buffer
   NVM.DATA0 = value;
   executeCommand( NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc );
   NVM.CMD = oldCmd;

   return true;
   // #]
}

Stream::Status Eeprom::write( uint16_t offset, void* pData, uint16_t length )
{
   // #[ operation write(uint16_t,void *,uint16_t)
   uint8_t* source = static_cast<uint8_t*>( pData );
   while ( length )
   {
      if ( !write( offset++, *source++ ) )
      {
         break;
      }
      length--;
   }

   return ( length ? Stream::ABORTED : Stream::SUCCESS );
   // #]
}

void Eeprom::fillBufferWithValue( uint8_t value )
{
   // #[ operation fillBufferWithValue(uint8_t)
   uint8_t oldCmd = NVM.CMD;

   flushBuffer();
   NvmController::waitWhileBusy();

   NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

   /*  Set address to zero, as only the lower bits matters. ADDR0 is
    *  maintained inside the loop below.
    */
   NVM.ADDR2 = 0x00;
   NVM.ADDR1 = 0x00;

   // Load multible bytes into page buffer
   for ( uint8_t i = 0; i < EEPROM_PAGE_SIZE; ++i )
   {
      NVM.ADDR0 = i;
      NVM.DATA0 = value;
   }
   NVM.CMD = oldCmd;
   // #]
}

bool Eeprom::isAddressInRange( uint16_t address )
{
   // #[ operation isAddressInRange(uint16_t)
   if ( ( address < EEPROM_START )
      || ( address > EEPROM_END ) )
   {
      ERROR_2( address, " isOutOfRange" );
      return false;
   }
   return true;
   // #]
}

/*********************************************************************
        File Path	: Xmega192A3/debug/Peripherals/Eeprom.cpp
*********************************************************************/
