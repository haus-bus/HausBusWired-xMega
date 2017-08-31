/*
 * Flash.h
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Flash_H
#define Peripherals_Flash_H

#include "NvmController.h"

#include <avr/pgmspace.h>


#ifndef _BOOTLOADER_SIZE_
#define _BOOTLOADER_SIZE_ 0
#endif


class Flash : public NvmController
{
   public:

    #if ( FLASHEND > USHRT_MAX )
      typedef uint32_t address_t;
    #else
      typedef uint16_t address_t;
    #endif

      ////    Operations    ////

      inline static void enableApplicationSection()
      {
         NVM_CMD = NVM_CMD_NO_OPERATION_gc;
      }

      inline static void eraseApplication()
      {
         waitWhileBusy();
         NVM_commonSPM( 0, NVM_CMD_ERASE_APP_gc );
      }

      inline static uint32_t getMaxApplicationSize()
      {
         return APP_SECTION_SIZE;
      }

      static uint16_t read( address_t address, void* pData, uint16_t length );

      static uint16_t write( address_t address, void* pData, uint16_t length );

      inline static uint8_t read( address_t& address )
      {
        #if ( FLASHEND > USHRT_MAX )
         return pgm_read_byte_far( address++ );
        #else
         return pgm_read_byte( address++ );
        #endif
      }

      inline static void writeBootSection( address_t address, void* pData, uint16_t length )
      {
         loadFlashPageBuffer( reinterpret_cast<uint8_t*>( pData ) );
         eraseWriteBootPage( address );
      }

   private:

      inline static void eraseApplicationPage( address_t address )
      {
         waitWhileBusy();
         NVM_commonSPM( address, NVM_CMD_ERASE_APP_PAGE_gc );
      }

      inline static void eraseWriteApplicationPage( uint32_t& address )
      {
         waitWhileBusy();
         NVM_commonSPM( address, NVM_CMD_ERASE_WRITE_APP_PAGE_gc );
      }

      ///**
      // * \brief Write a page within the application section
      // *
      // * Write a page within the application section with the data stored in the
      // * page buffer. The page needs to be erased before the write to avoid
      // * corruption of the data written.
      // *
      // * \param page_addr Byte address to the page to delete
      // */
      inline static void writeApplicationPage( address_t& address )
      {
         waitWhileBusy();
         NVM_commonSPM( address, NVM_CMD_WRITE_APP_PAGE_gc );
      }
};

#endif

