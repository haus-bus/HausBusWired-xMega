/*
 * NvmController.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_NvmController_H
#define Peripherals_NvmController_H

#include "Peripherals.h"

#include "NvmLowLevel.inc"

class NvmController
{
   public:

      ////    Operations    ////

      inline static void eraseUserSignature()
      {
         waitWhileBusy();
         NVM_commonSPM( 0, NVM_CMD_ERASE_USER_SIG_ROW_gc );
      }

      inline static uint32_t getApplicationCRC()
      {
         executeCommand( NVM_CMD_APP_CRC_gc );
         uint32_t crc = ( (uint32_t)NVM.DATA2 << 16 ) | ( (uint16_t)NVM.DATA1 << 8 ) | NVM.DATA0;
         NVM_CMD = NVM_CMD_NO_OPERATION_gc;
         return crc;
      }

      inline static uint32_t getBootCRC()
      {
         executeCommand( NVM_CMD_BOOT_CRC_gc );
         uint32_t crc = ( (uint32_t)NVM.DATA2 << 16 ) | ( (uint16_t)NVM.DATA1 << 8 ) | NVM.DATA0;
         NVM_CMD = NVM_CMD_NO_OPERATION_gc;
         return crc;
      }

      inline static uint32_t getMaxBooterSize()
      {
         return BOOT_SECTION_SIZE;
      }

      inline static uint16_t getPageSize()
      {
         return APP_SECTION_PAGE_SIZE;
      }

      inline static uint32_t getRangeCRC( const uint32_t& start, const uint32_t& end )
      {
         // Set address.
         NVM.ADDR0 = start & 0xFF;
         NVM.ADDR1 = ( start >> 8 ) & 0xFF;
         NVM.ADDR2 = ( start >> 16 ) & 0xFF;

         NVM.DATA0 = end & 0xFF;
         NVM.DATA1 = ( end >> 8 ) & 0xFF;
         NVM.DATA2 = ( end >> 16 ) & 0xFF;

         executeCommand( NVM_CMD_FLASH_RANGE_CRC_gc );
         uint32_t crc = ( (uint32_t)NVM.DATA2 << 16 ) | ( (uint16_t)NVM.DATA1 << 8 ) | NVM.DATA0;
         NVM_CMD = NVM_CMD_NO_OPERATION_gc;
         return crc;
      }

      inline static uint8_t readCalibrationByte( uint8_t index )
      {
         return NVM_readByte( NVM_CMD_READ_CALIB_ROW_gc, index );
      }

      inline static uint8_t readDeviceRev()
      {
         return MCU.REVID;
      }

      inline static uint8_t readFuseByte( uint8_t index )
      {
         waitWhileBusy();

         // Set address
         NVM.ADDR0 = index;

         // Issue READ_FUSES command
         executeCommand( NVM_CMD_READ_FUSES_gc );

         return NVM.DATA0;
      }

      inline static uint8_t readUserSignature( uint16_t index )
      {
         return NVM_readByte( NVM_CMD_READ_USER_SIG_ROW_gc, index );
      }

      inline static void waitWhileBusy()
      {
         while ( ( NVM.STATUS & NVM_NVMBUSY_bm ) == NVM_NVMBUSY_bm )
         {
         }
      }

      static void executeCommand( NVM_CMD_t command );

      static uint16_t readUserSignature( uint16_t index, void* pData, uint16_t length );

      static uint16_t writeUserSignature( uint16_t index, void* pData, uint16_t length );

   protected:

      inline static void eraseBootPage( uint32_t& address )
      {
         waitWhileBusy();
         NVM_commonSPM( address, NVM_CMD_ERASE_BOOT_PAGE_gc );
      }

      inline static void eraseWriteBootPage( uint32_t& address )
      {
         waitWhileBusy();
         NVM_commonSPM( address, NVM_CMD_ERASE_WRITE_BOOT_PAGE_gc );
      }

      ///**
      // * \brief Flush flash page buffer
      // *
      // * Clear the NVM controller page buffer for flash. This needs to be called
      // * before using \ref NVM_loadWordToFlashBuffer if it has not already been
      // * cleared.
      // *
      // */
      inline static void flushBuffer()
      {
         waitWhileBusy();
         NVM_commonSPM( 0, NVM_CMD_ERASE_FLASH_BUFFER_gc );
      }

      static void loadFlashPageBuffer( uint8_t* data );

      inline static void writeBootPage( uint32_t& address )
      {
         waitWhileBusy();
         NVM_commonSPM( address, NVM_CMD_WRITE_BOOT_PAGE_gc );
      }
};

#endif
