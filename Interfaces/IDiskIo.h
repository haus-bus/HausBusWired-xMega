/*
 * IDiskIo.h
 *
 *  Created on: 04.04.2016
 *      Author: viktor.pankraz
 */

#ifndef IDISKIO_H_
#define IDISKIO_H_

#include <stdint.h>

class IDiskIo
{

   public:
      // Results of Disk Functions
      enum Results
      {
         OK = 0,      // Successful
         ERROR,       // R/W Error
         WRITE_PROTECTED, // Write Protected
         NOT_READY,   // Not Ready
         PARAMETER_ERROR // Invalid Parameter
      };

      class Status
      {
         public:

            inline Status()
            {
               clear();
            }

            inline void clear()
            {
               status.status = 0;
            }

            inline void setNotInitialized()
            {
               status.bits.notInitialized = 1;
            }

            inline void setNoDisk()
            {
               status.bits.noDisk = 1;
            }

            inline void setProtected()
            {
               status.bits.protection = 1;
            }

            inline uint8_t hasError()
            {
               return status.status;
            }

            inline bool isInitialized()
            {
               return !status.bits.notInitialized;
            }

            inline bool isDiskAvailable()
            {
               return !status.bits.noDisk;
            }

            inline bool isProtected()
            {
               return status.bits.protection;
            }

         private:

            struct Bits
            {
               uint8_t notInitialized : 1;
               uint8_t noDisk : 1;
               uint8_t protection : 1;
            };

            union uStatus
            {
               uint8_t status;
               Bits bits;
            };

            uStatus status;
      };

      // Command code for disk_ioctrl function
      enum GenericCommand
      {

         // Generic command (defined for FatFs)
         CTRL_SYNC,     // Flush disk cache
         GET_SECTOR_COUNT, // Get media size
         GET_SECTOR_SIZE, // Get sector size
         GET_ERASE_BLOCK_SIZE, // Get erase block size
         CTRL_ERASE_SECTOR, // Force erased a block of sectors

         // Generic command
         CTRL_POWER,  // Get/Set power status
         CTRL_LOCK,   // Lock/Unlock media removal
         CTRL_EJECT,  // Eject media

         // MMC/SDC specific ioctl command
         MMC_GET_TYPE, // Get card type
         MMC_GET_CSD, // Get CSD
         MMC_GET_CID, // Get CID
         MMC_GET_OCR, // Get OCR
         MMC_GET_SDSTAT, // Get SD status

         // ATA/CF specific ioctl command
         ATA_GET_REV = 20, // Get F/W revision
         ATA_GET_MODEL, // Get model name
         ATA_GET_SN,  // Get serial number

         // NAND specific ioctl command
         NAND_FORMAT = 30 // Create physical format
      };

      /**
       * \brief Initialize a disk.
       *
       * \return 0 or disk status in combination of StatusBits
       */
      virtual Status init() = 0;

      /**
       * \brief Initialize a disk.
       *
       * \return 0 or disk status in combination of StatusBits
       */
      virtual Status getStatus() = 0;

      /**
       * \brief  Read sector(s).
       *
       * \param buffer Data buffer to store read data.
       * \param sector Sector address (LBA).
       * \param count Number of sectors to read (1..255).
       *
       * \return Result::OK for success, otherwise Result error code.
       */
      virtual Results read( uint8_t* buffer, uint32_t sector, uint8_t count ) = 0;

      /**
       * \brief  Write sector(s).
       *
       * The FatFs module will issue multiple sector transfer request (count > 1) to
       * the disk I/O layer. The disk function should process the multiple sector
       * transfer properly. Do not translate it into multiple sector transfers to the
       * media, or the data read/write performance may be drastically decreased.
       *
       * \param buff Data buffer to store read data.
       * \param sector Sector address (LBA).
       * \param count Number of sectors to read (1..255).
       *
       * \return Result::OK for success, otherwise Result error code.
       */
      virtual Results write( const uint8_t* buffer, uint32_t sector,
                             uint8_t count ) = 0;

      /**
       * \brief  Miscellaneous functions, which support the following commands:
       *
       * CTRL_SYNC    Make sure that the disk drive has finished pending write
       * process. When the disk I/O module has a write back cache, flush the
       * dirty sector immediately.
       * In read-only configuration, this command is not needed.
       *
       * GET_SECTOR_COUNT    Return total sectors on the drive into the DWORD variable
       * pointed by buffer.
       * This command is used only in f_mkfs function.
       *
       * GET_BLOCK_SIZE    Return erase block size of the memory array in unit
       * of sector into the DWORD variable pointed by Buffer.
       * When the erase block size is unknown or magnetic disk device, return 1.
       * This command is used only in f_mkfs function.
       *
       * GET_SECTOR_SIZE    Return sector size of the memory array.
       *
       * \param cmd Control code.
       * \param buffer Buffer to send/receive control data.
       *
       * \return Result::OK for success, otherwise Result error code.
       */
      virtual Results ioctl( GenericCommand cmd, void* buffer ) = 0;

};

#endif /* IDISKIO_H_ */
