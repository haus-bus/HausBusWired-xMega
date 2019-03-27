/*
 * SdCard.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "SdCard.h"

#include <FlashString.h>
#include <Peripherals/Spi.h>
#include <IStream.h>

#define getId() FSTR( "SdCard::" )

const uint8_t SdCard::debugLevel( DEBUG_LEVEL_OFF );

SdCard::SdCard( Spi& _spi, DigitalOutput _chipSelectPin ) :
   spi( &_spi ), chipSelectPin( _chipSelectPin ), type( 0 )
{
   deselect();
}

uint8_t SdCard::getInfo( SdCard::Info* info )
{
   if ( !info )
   {
      return 0;
   }

   memset( info, 0, sizeof( *info ) );

   select();

   // read cid register
   if ( sendCommand( SEND_CID, 0 ) )
   {
      deselect();
      return 0;
   }

   if ( waitForDataBlockStarts() == 0 )
   {
      deselect();
      return 0;
   }

   for ( uint8_t i = 0; i < 18; ++i )
   {
      uint8_t b;
      spi->read( b );

      switch ( i )
      {
         case 0:
            info->manufacturer = b;
            break;
         case 1:
         case 2:
            info->oem[i - 1] = b;
            break;
         case 3:
         case 4:
         case 5:
         case 6:
         case 7:
            info->product[i - 3] = b;
            break;
         case 8:
            info->revision = b;
            break;
         case 9:
         case 10:
         case 11:
         case 12:
            info->serial |= (uint32_t) b << ( ( 12 - i ) * 8 );
            break;
         case 13:
            info->manufacturingYear = b << 4;
            break;
         case 14:
            info->manufacturingYear |= b >> 4;
            info->manufacturingMonth = b & 0x0f;
            break;
      }
   }

   // read csd register
   uint8_t csdReadBlockLen = 0;
   uint8_t csdCapacitzMultiplier = 0;
   uint32_t csdCapacity = 0;

   if ( sendCommand( SEND_CSD, 0 ) )
   {
      deselect();
      return 0;
   }

   if ( waitForDataBlockStarts() == 0 )
   {
      deselect();
      return 0;
   }

   for ( uint8_t i = 0; i < 18; ++i )
   {
      uint8_t b;
      spi->read( b );

      if ( i == 14 )
      {
         if ( b & 0x40 )
         {
            info->flagCopy = 1;
         }
         if ( b & 0x20 )
         {
            info->flagWriteProtect = 1;
         }
         if ( b & 0x10 )
         {
            info->flagWriteProtectTemp = 1;
         }
         info->format = ( b & 0x0c ) >> 2;
      }
      else
      {
         if ( type & ( 1 << SD_RAW_SPEC_2 ) )
         {
            switch ( i )
            {
               case 7:
                  b &= 0x3f;
               case 8:
               case 9:
                  csdCapacity <<= 8;
                  csdCapacity |= b;
                  break;
            }
            if ( i == 9 )
            {
               ++csdCapacity;
               info->capacity = (offset_t) csdCapacity * BLOCK_SIZE * 1024;
            }
         }
         else
         {
            switch ( i )
            {
               case 5:
                  csdReadBlockLen = b & 0x0f;
                  break;
               case 6:
                  csdCapacity = b & 0x03;
                  csdCapacity <<= 8;
                  break;
               case 7:
                  csdCapacity |= b;
                  csdCapacity <<= 2;
                  break;
               case 8:
                  csdCapacity |= b >> 6;
                  ++csdCapacity;
                  break;
               case 9:
                  csdCapacitzMultiplier = b & 0x03;
                  csdCapacitzMultiplier <<= 1;
                  break;
               case 10:
                  csdCapacitzMultiplier |= b >> 7;

                  info->capacity = (uint32_t) csdCapacity
                                   << ( csdCapacitzMultiplier + csdReadBlockLen + 2 );

                  break;
            }
         }
      }
   }

   deselect();

   return 1;
}

IDiskIo::Status SdCard::init()
{
   DEBUG_H1( FSTR( "init sdCard..." ) );
   Status status;

   // initialize SPI with lowest frequency;
   // max. 400kHz during identification mode of card
   spi->init<true, SPI_MODE_0_gc, false, SPI_PRESCALER_DIV128_gc, false>();

   deselect();

   // card needs 74 cycles minimum to start up
   for ( uint8_t i = 0; i < 10; i++ )
   {
      // wait 8 clock cycles
      uint8_t dummy = 0xFF;
      spi->write( dummy );
   }

   // reset card
   select();

   uint8_t response;
   for ( uint16_t i = 0;; i++ )
   {
      response = sendCommand( GO_IDLE_STATE, 0 );
      if ( response == ( 1 << R1_IDLE_STATE ) )
      {
         break;
      }
      if ( i == 0x1ff )
      {
         deselect();
         ERROR_2( FSTR( "reset failed, response=0x" ), response );
         status.setNoDisk();
         return status;
      }
   }

   status.setNotInitialized();

   // check for version of SD card specification
   response = sendCommand( SEND_IF_COND, 0x100   // 2.7V - 3.6V
                           | 0xaa ); // test pattern
   if ( ( response & ( 1 << R1_ILL_COMMAND ) ) == 0 )
   {
      uint8_t data;
      spi->read( data );
      spi->read( data );
      spi->read( data );
      if ( ( data & 0x01 ) == 0 )
      {
         ERROR_1( FSTR( "card operation voltage range doesn't match" ) );
         return status;
      }
      spi->read( data );
      if ( data != 0xaa )
      {
         ERROR_1( FSTR( "wrong test pattern" ) );
         return status;
      }

      // card conforms to SD 2 card specification
      type |= ( 1 << SD_RAW_SPEC_2 );
      DEBUG_M1( FSTR( "found SD 2 card" ) );
   }
   else
   {
      // determine SD/MMC card type
      sendCommand( APP, 0 );
      response = sendCommand( SD_SEND_OP_COND, 0 );
      if ( ( response & ( 1 << R1_ILL_COMMAND ) ) == 0 )
      {
         // card conforms to SD 1 card specification
         type |= ( 1 << SD_RAW_SPEC_1 );
         DEBUG_M1( FSTR( "found SD 1 card" ) );
      }
      else
      {
         // MMC card
         DEBUG_M1( FSTR( "found MMC card" ) );
      }
   }

   // wait for card to get ready
   for ( uint16_t i = 0;; ++i )
   {
      if ( type & ( ( 1 << SD_RAW_SPEC_1 ) | ( 1 << SD_RAW_SPEC_2 ) ) )
      {
         uint32_t arg = 0;
         if ( type & ( 1 << SD_RAW_SPEC_2 ) )
         {
            arg = 0x40000000;
         }
         sendCommand( APP, 0 );
         response = sendCommand( SD_SEND_OP_COND, arg );
      }
      else
      {
         response = sendCommand( SEND_OP_COND, 0 );
      }

      if ( ( response & ( 1 << R1_IDLE_STATE ) ) == 0 )
      {
         break;
      }
      if ( i == 0x7fff )
      {
         deselect();
         ERROR_1( FSTR( "card not ready timeout" ) );
         return status;
      }
   }

   if ( type & ( 1 << SD_RAW_SPEC_2 ) )
   {
      if ( sendCommand( READ_OCR, 0 ) )
      {
         deselect();
         ERROR_1( FSTR( "command READ_OCR failed" ) );
         return status;
      }
      uint8_t data;
      spi->read( data );
      if ( data & 0x40 )
      {
         type |= ( 1 << SD_RAW_SPEC_SDHC );
      }
      spi->read( data );
      spi->read( data );
      spi->read( data );
   }

   // set block size to 512 bytes */
   if ( sendCommand( SET_BLOCKLEN, BLOCK_SIZE ) )
   {
      deselect();
      ERROR_1( FSTR( "failed to set block size to 512 bytes" ) );
      return status;
   }

   deselect();

   // switch to highest SPI frequency possible
   spi->init<true, SPI_MODE_0_gc, true, SPI_PRESCALER_DIV4_gc, false>();

   status.clear();
   return status;
}

IDiskIo::Status SdCard::getStatus()
{
   DEBUG_H1( FSTR( "getStatus()" ) );
   Status status;

   SdCard::Info info;
   if ( !getInfo( &info ) )
   {
      status.setNoDisk();
   }
   DEBUG_M2( FSTR( "returns 0x" ), status.hasError() );
   return status;
}

IDiskIo::Results SdCard::read( uint8_t* buffer, uint32_t sector, uint8_t count )
{
   DEBUG_H4( FSTR( "read at 0x" ), sector * BLOCK_SIZE, FSTR( ", size 0x" ),
             BLOCK_SIZE * count );

   while ( count-- > 0 )
   {
      DEBUG_M2( FSTR( "read block 0x" ), sector );

      // address card
      select();

      // send single block request
      if ( sendCommand(
              READ_SINGLE_BLOCK,
              ( type & ( 1 << SD_RAW_SPEC_SDHC ) ? sector : sector* BLOCK_SIZE ) ) )
      {
         ERROR_1( FSTR( "sendCommand READ_SINGLE_BLOCK failed!" ) );
         deselect();
         return ERROR;
      }

      if ( waitForDataBlockStarts() == 0 )
      {
         deselect();
         return NOT_READY;
      }

      if ( spi->read( buffer, BLOCK_SIZE ) != IStream::SUCCESS )
      {
         ERROR_1( FSTR( "read sector failed!" ) );
         return ERROR;
      }

      // read crc16
      uint8_t data;
      spi->read( data );
      spi->read( data );

      deselect();

      // let card some time to finish
      spi->read( data );

      buffer += BLOCK_SIZE;
      sector++;
   }

   return OK;
}

IDiskIo::Results SdCard::write( const uint8_t* buffer, uint32_t sector,
                                uint8_t count )
{
   DEBUG_H4( FSTR( "write at 0x" ), sector * BLOCK_SIZE, FSTR( ", size 0x" ),
             BLOCK_SIZE * count );

   while ( count-- > 0 )
   {
      DEBUG_M2( FSTR( "write block 0x" ), sector );

      select();

      // send single block request
      if ( sendCommand(
              WRITE_SINGLE_BLOCK,
              ( type & ( 1 << SD_RAW_SPEC_SDHC ) ? sector : sector* BLOCK_SIZE ) ) )
      {
         ERROR_1( FSTR( "sendCommand WRITE_SINGLE_BLOCK failed!" ) );
         deselect();
         return ERROR;
      }

      // send start byte
      uint8_t data = 0xFE;
      spi->write( data );

      // write byte block
      if ( spi->write( buffer, BLOCK_SIZE ) != IStream::SUCCESS )
      {
         ERROR_1( FSTR( "write sector failed!" ) );
         return ERROR;
      }

      // write dummy crc16
      spi->read( data );  // send 0xff
      spi->read( data );  // send 0xff

      // wait while card is busy
      do
      {
         spi->read( data );
      }
      while ( data != 0xFF );

      spi->read( data );

      // deaddress card
      deselect();

      buffer += BLOCK_SIZE;
      sector++;
   }

   return OK;
}

IDiskIo::Results SdCard::ioctl( GenericCommand cmd, void* buffer )
{
   DEBUG_H3( FSTR( "getStatus(" ), (uint8_t )cmd, ')' );

   Results result = PARAMETER_ERROR;

   switch ( cmd )
   {
      case GET_ERASE_BLOCK_SIZE:
      {
         *(uint32_t*) buffer = 1;
         result = OK;
         break;
      }

      // Get the number of sectors on the disk (DWORD)
      case GET_SECTOR_COUNT:
      {
         SdCard::Info info;
         if ( getInfo( &info ) )
         {
            *(uint32_t*) buffer = info.getCapacity() / BLOCK_SIZE;
            result = OK;
         }
         else
         {
            result = NOT_READY;
         }
         break;
      }

      // Get sectors on the disk (WORD)
      case GET_SECTOR_SIZE:
      {
         *(uint16_t*) buffer = BLOCK_SIZE;
         result = OK;
         break;
      }

      // Make sure that data has been written
      case CTRL_SYNC:
      {
         // no cache for the moment
         result = OK;
         break;
      }

      default:
         result = PARAMETER_ERROR;
   }

   DEBUG_M2( FSTR( "returns 0x" ), *(uint32_t* ) buffer );
   return result;
}

uint8_t SdCard::sendCommand( uint8_t command, uint32_t arg )
{
   // wait some clock cycles
   uint8_t data;
   spi->read( data );

   // send command via SPI
   data = ( 0x40 | command );
   spi->write( data );

   spi->write( ( (convert_u*) &arg )->byte[3] );
   spi->write( ( (convert_u*) &arg )->byte[2] );
   spi->write( ( (convert_u*) &arg )->byte[1] );
   spi->write( ( (convert_u*) &arg )->byte[0] );

   switch ( command )
   {
      case GO_IDLE_STATE:
         data = 0x95;
         break;

      case SEND_IF_COND:
         data = 0x87;
         break;

      default:
         data = 0xFF;
         break;
   }

   spi->write( data );

   // receive response
   for ( uint8_t i = 0; i < 10; ++i )
   {
      spi->read( data );
      if ( data != 0xff )
      {
         break;
      }
   }

   return data;
}

uint8_t SdCard::waitForDataBlockStarts()
{
   // wait for data block (start byte 0xfe)
   uint16_t timeout = 20000;
   uint8_t data;
   do
   {
      spi->read( data );
      timeout--;
   }
   while ( ( data != 0xFE ) && timeout );

   if ( timeout == 0 )
   {
      ERROR_1( FSTR( "read timeout reached!" ) );
      return 0;
   }
   return 1;
}

void SdCard::Info::print()
{
   DEBUG_H1( FSTR( "Info::print()" ) );

   DEBUG_M2( FSTR( "manufacturer :" ), manufacturer );
   DEBUG_M2( FSTR( "oem          :" ), (const char* )oem );
   DEBUG_M2( FSTR( "product      :" ), (const char* )product );
   DEBUG_M2( FSTR( "revision     :" ), revision );
   DEBUG_M2( FSTR( "serial       :" ), serial );
   DEBUG_M2( FSTR( "manuf.-year  :" ), manufacturingYear );
   DEBUG_M2( FSTR( "manuf.-month :" ), manufacturingMonth );
   DEBUG_M2( FSTR( "capacity     :" ), capacity );
   DEBUG_M2( FSTR( "format       :" ), format );
   DEBUG_M2( FSTR( "flagCopy     :" ), flagCopy );
   DEBUG_M2( FSTR( "flagWriteProtect     :" ), flagWriteProtect );
   DEBUG_M2( FSTR( "flagWriteProtectTemp :" ), flagWriteProtectTemp );
}
