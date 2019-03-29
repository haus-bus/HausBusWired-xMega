/*
 * SdCard.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_IntegratedCircuits_SdCard_H
#define Electronics_IntegratedCircuits_SdCard_H

#include <IDiskIo.h>
#include <DigitalOutput.h>


class Spi;


class SdCard : public IDiskIo
{
   public:

      class Info;

      enum CardTypes
      {
         SD_RAW_SPEC_1,
         SD_RAW_SPEC_2,
         SD_RAW_SPEC_SDHC
      };

      enum Commands
      {
         GO_IDLE_STATE = 0x00,    // response R1
         SEND_OP_COND,            // response R1
         SEND_IF_COND = 0x08,     // response R7
         SEND_CSD,                // response R1
         SEND_CID,                // response R1
         STOP_TRANSMISSION = 0x0C, // response R1
         SEND_STATUS,             // response R2
         SET_BLOCKLEN = 0x10,     // arg0[31:0]: block length, response R1
         READ_SINGLE_BLOCK,       // arg0[31:0]: data address, response R1
         READ_MULTIPLE_BLOCK,     // arg0[31:0]: data address, response R1
         WRITE_SINGLE_BLOCK = 0x18, // arg0[31:0]: data address, response R1
         WRITE_MULTIPLE_BLOCK,    // arg0[31:0]: data address, response R1
         PROGRAM_CSD = 0x1B,      // response R1
         SET_WRITE_PROT,          // arg0[31:0]: data address, response R1b
         CLR_WRITE_PROT,          // arg0[31:0]: data address, response R1b
         SEND_WRITE_PROT,         // arg0[31:0]: write protect data address, response R1
         TAG_SECTOR_START = 0x20, // arg0[31:0]: data address, response R1
         TAG_SECTOR_END,          // arg0[31:0]: data address, response R1
         UNTAG_SECTOR,            // arg0[31:0]: data address, response R1
         TAG_ERASE_GROUP_START,   // arg0[31:0]: data address, response R1
         TAG_ERASE_GROUP_END,     // arg0[31:0]: data address, response R1
         UNTAG_ERASE_GROUP,       // arg0[31:0]: data address, response R1
         ERASE,                   // arg0[31:0]: stuff bits, response R1b
         SD_SEND_OP_COND = 0x29,  // arg0[31:0]: OCR contents, response R1
         LOCK_UNLOCK,             // arg0[31:0]: stuff bits, response R1b
         APP = 0x37,              // arg0[31:0]: stuff bits, response R1
         READ_OCR = 0x3A,         // arg0[31:0]: stuff bits, response R3
         CRC_ON_OFF               // arg0[31:1]: stuff bits, arg0[0:0]: crc option, response R1
      };

      enum CommandResponses
      {
         R1_IDLE_STATE = 0,
         R1_ERASE_RESET,
         R1_ILL_COMMAND,
         R1_COM_CRC_ERR,
         R1_ERASE_SEQ_ERR,
         R1_ADDR_ERR,
         R1_PARAM_ERR,
         R2_CARD_LOCKED = 0,
         R2_WP_ERASE_SKIP,
         R2_ERR,
         R2_CARD_ERR,
         R2_CARD_ECC_FAIL,
         R2_WP_VIOLATION,
         R2_INVAL_ERASE,
         R2_OUT_OF_RANGE,
         R2_CSD_OVERWRITE = 7,
         R2_IDLE_STATE,
         R2_ERASE_RESET,
         R2_ILL_COMMAND,
         R2_COM_CRC_ERR,
         R2_ERASE_SEQ_ERR,
         R2_ADDR_ERR,
         R2_PARAM_ERR,
         R3_IDLE_STATE = 32,
         R3_ERASE_RESET,
         R3_ILL_COMMAND,
         R3_COM_CRC_ERR,
         R3_ERASE_SEQ_ERR,
         R3_ADDR_ERR,
         R3_PARAM_ERR
      };

      enum DataResponses
      {
         STATUS_MASK = 0x0E,
         ACCEPTED = 0x05,
         CRC_ERROR = 0x0A,
         WRITE_ERROR = 0x0C
      };

#if SD_RAW_SDHC
      typedef uint64_t offset_t;
#else
      typedef uint32_t offset_t;
#endif

      enum FileSystems
      {
         HARDDISK = 0, // The card's layout is harddisk-like, which means it contains
                       // a master boot record with a partition table.
         SUPERFLOPPY, // The card contains a single filesystem and no partition table.
         UNIVERSAL, // The card's layout follows the Universal File Format.
         UNKNOWN    // The card's layout is unknown.
      };

      static const uint16_t BLOCK_SIZE = 512;

      class Info
      {
         ////    Additional operations    ////

         public:

            void print();

            inline uint8_t getManufacturer() const
            {
               return manufacturer;
            }

            inline void setManufacturer( uint8_t p_manufacturer )
            {
               manufacturer = p_manufacturer;
            }

            inline uint8_t getOem( int i1 ) const
            {
               return oem[i1];
            }

            inline void setOem( int i1, uint8_t p_oem )
            {
               oem[i1] = p_oem;
            }

            inline uint8_t getProduct( int i1 ) const
            {
               return product[i1];
            }

            inline void setProduct( int i1, uint8_t p_product )
            {
               product[i1] = p_product;
            }

            inline uint8_t getRevision() const
            {
               return revision;
            }

            inline void setRevision( uint8_t p_revision )
            {
               revision = p_revision;
            }

            inline uint32_t getSerial() const
            {
               return serial;
            }

            inline void setSerial( uint32_t p_serial )
            {
               serial = p_serial;
            }

            inline uint8_t getManufacturing_year() const
            {
               return manufacturingYear;
            }

            inline void setManufacturing_year( uint8_t p_manufacturing_year )
            {
               manufacturingYear = p_manufacturing_year;
            }

            inline uint8_t getManufacturing_month() const
            {
               return manufacturingMonth;
            }

            inline void setManufacturing_month( uint8_t p_manufacturing_month )
            {
               manufacturingMonth = p_manufacturing_month;
            }

            inline offset_t getCapacity() const
            {
               return capacity;
            }

            inline void setCapacity( offset_t p_capacity )
            {
               capacity = p_capacity;
            }

            inline uint8_t getFlag_copy() const
            {
               return flagCopy;
            }

            inline void setFlag_copy( uint8_t p_flag_copy )
            {
               flagCopy = p_flag_copy;
            }

            inline uint8_t getFlag_write_protect() const
            {
               return flagWriteProtect;
            }

            inline void setFlag_write_protect( uint8_t p_flag_write_protect )
            {
               flagWriteProtect = p_flag_write_protect;
            }

            inline uint8_t getFlag_write_protect_temp() const
            {
               return flagWriteProtectTemp;
            }

            inline void setFlag_write_protect_temp( uint8_t p_flag_write_protect_temp )
            {
               flagWriteProtectTemp = p_flag_write_protect_temp;
            }

            inline uint8_t getFormat() const
            {
               return format;
            }

            inline void setFormat( uint8_t p_format )
            {
               format = p_format;
            }

            ////    Attributes    ////

            // A manufacturer code globally assigned by the SD card organization.
            uint8_t manufacturer;

            // A string describing the card's OEM or content, globally assigned by the SD card organization.
            uint8_t oem[3];

            // A product name.
            uint8_t product[6];

            // The card's revision, coded in packed BCD.
            // For example, the revision value 0x32 means "3.2".
            uint8_t revision;

            // A serial number assigned by the manufacturer.
            uint32_t serial;

            // The year of manufacturing. A value of zero means year 2000.
            uint8_t manufacturingYear;

            // The month of manufacturing.
            uint8_t manufacturingMonth;

            // The card's total capacity in bytes.
            offset_t capacity;

            // Defines wether the card's content is original or copied.
            // A value of 0 means original, 1 means copied.
            uint8_t flagCopy;

            // Defines wether the card's content is write-protected.
            // This is an internal flag and does not represent the state of the card's mechanical write-protect switch.
            uint8_t flagWriteProtect;

            // Defines wether the card's content is temporarily write-protected.
            // This is an internal flag and does not represent the state of the card's mechanical write-protect switch.
            uint8_t flagWriteProtectTemp;

            // The card's data layout.
            uint8_t format;
      } __attribute__( ( packed ) );

      ////    Constructors and destructors    ////

      SdCard( Spi& _spi, DigitalOutput _chipSelectPin );

      ////    Operations    ////


      uint8_t getInfo( Info* info );

      virtual Status init();

      virtual Status getStatus();

      virtual Results read( uint8_t* buffer, uint32_t sector, uint8_t count );

      virtual Results write( const uint8_t* buffer, uint32_t sector, uint8_t count );

      virtual Results ioctl( GenericCommand cmd, void* buffer );

   private:

      inline void deselect();

      inline void select();

      uint8_t sendCommand( uint8_t command, uint32_t arg );

      uint8_t waitForDataBlockStarts();

      ////    Additional operations    ////

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      inline Spi* getSpi() const
      {
         return spi;
      }

   private:

      inline DigitalOutput* getChipSelectPin() const
      {
         return (DigitalOutput*) &chipSelectPin;
      }

      inline uint8_t getType()
      {
         return type;
      }

      inline void setType( uint8_t p_type )
      {
         type = p_type;
      }

      ////    Attributes    ////

   protected:

      static const uint8_t debugLevel;

      Spi* spi;

   private:

      DigitalOutput chipSelectPin;

      uint8_t type;

};

inline void SdCard::deselect()
{
   chipSelectPin.set();
}

inline void SdCard::select()
{
   chipSelectPin.clear();
}

#endif
