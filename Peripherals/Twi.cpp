/*
 * Twi.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Peripherals/Twi.h"

const uint8_t Twi::debugLevel( DEBUG_LEVEL_OFF );

uint8_t Twi::Master::handleStatus( uint8_t& data, bool finish )
{
   uint8_t const status = reg.STATUS;

   if ( status & TWI_MASTER_ARBLOST_bm )
   {
      reg.STATUS = status;
      reg.CTRLC = TWI_MASTER_CMD_STOP_gc;
      DEBUG_L1( "AEP" )
      return ERR_BUSY;
   }
   else if ( ( status & TWI_MASTER_BUSERR_bm ) || ( status & TWI_MASTER_RXACK_bm ) )
   {
      DEBUG_L1( ( ( status & TWI_MASTER_BUSERR_bm ) ? "BEP" : "NP" ) );
      reg.STATUS = status;
      reg.CTRLC = TWI_MASTER_CMD_STOP_gc;
      return ERR_IO_ERROR;
   }
   else if ( status & TWI_MASTER_WIF_bm )
   {
      if ( !finish )
      {
         reg.DATA = data;
         DEBUG_L1( 'D' );
      }
      else
      {
         // Send STOP condition to complete the transaction.
         reg.CTRLC = TWI_MASTER_CMD_STOP_gc;
         DEBUG_L1( 'P' );
         return STATUS_STOP;
      }
   }
   else if ( status & TWI_MASTER_RIF_bm )
   {
      data = reg.DATA;
      DEBUG_L1( 'D' );

      // If there is more to read, issue ACK and start a byte read.
      // Otherwise, issue NACK and STOP to complete the transaction.
      if ( !finish )
      {
         reg.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
      }
      else
      {
         reg.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
         DEBUG_L1( 'P' );
         return STATUS_STOP;
      }
   }
   else
   {
      ERROR_1( "M.ERR_PROTOCOL" );
      return ERR_PROTOCOL;
   }
   return STATUS_OK;
}

uint16_t Twi::Master::write( uint8_t address, void* pData, uint16_t length, bool stop )
{
   if ( getBusState() != TWI_MASTER_BUSSTATE_IDLE_gc )
   {
      return 0;
   }

   uint8_t* data = static_cast<uint8_t*>( pData );
   uint16_t bytesTransferred = 0;

   startTransfer( address << 1 );
   waitNewStatusAvailable();

   while ( !hasError() && bytesTransferred < length )
   {
      writeByte( data[bytesTransferred] );
      waitNewStatusAvailable();
      if ( hasError() )
      {
         break;
      }
      bytesTransferred++;
   }

   if ( stop || hasError() )
   {
      stopWriteTransfer();
   }
   return bytesTransferred;
}

uint16_t Twi::Master::read( uint8_t address, void* pData, uint16_t length, bool stop )
{
   if ( getBusState() != TWI_MASTER_BUSSTATE_IDLE_gc )
   {
      return 0;
   }

   uint8_t* data = static_cast<uint8_t*>( pData );
   uint16_t bytesTransferred = 0;

   startTransfer( ( address << 1 ) | 1 );
   waitNewStatusAvailable();

   while ( !hasError() && bytesTransferred < length )
   {
      reg.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
      waitNewStatusAvailable();
      if ( hasError() )
      {
         break;
      }
      data[bytesTransferred++] = readByte();
   }

   if ( stop || hasError() )
   {
      stopReadTransfer();
   }
   return bytesTransferred;

}

uint8_t Twi::Slave::handleStatus( uint8_t& data, bool finish )
{
   const uint8_t status = reg.STATUS;

   // If bus error.
   if ( status & TWI_SLAVE_BUSERR_bm )
   {
      reg.STATUS = TWI_SLAVE_BUSERR_bm;
      DEBUG_L1( "be" );
      return ERR_IO_ERROR;
   }
   // If transmit collision.
   else if ( status & TWI_SLAVE_COLL_bm )
   {
      reg.STATUS = TWI_SLAVE_COLL_bm;
      DEBUG_L1( "ce" );
      // handleStop();
      return ERR_BUSY;
   }
   // If address match.
   else if ( ( status & TWI_SLAVE_APIF_bm ) && ( status & TWI_SLAVE_AP_bm ) )
   {
      clearErrors();

      // Fetch address
      data = reg.DATA;
      DEBUG_L1( 'm' );

      // If application signalling need to abort (error occured).
      if ( finish )
      {
         reg.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
         DEBUG_L1( 'n' );
      }
      else
      {
         // Disable stop interrupt.
         reg.CTRLA &= ~TWI_SLAVE_PIEN_bm;

         // Send ACK, wait for data interrupt.
         reg.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
         DEBUG_L1( 'a' );
      }
      return STATUS_START;
   }
   // If stop (only enabled through slave read transaction).
   else if ( status & TWI_SLAVE_APIF_bm )
   {
      handleStop();
      DEBUG_L1( 'p' );
      return STATUS_STOP;
   }
   // If data interrupt.
   else if ( status & TWI_SLAVE_DIF_bm )
   {
      DEBUG_L1( 'd' );
      if ( status & TWI_SLAVE_DIR_bm )
      {
         // If NACK, slave write transaction finished. */
         if ( finish || ( status & TWI_SLAVE_RXACK_bm ) )
         {
            reg.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
         }
         // If ACK, master expects more data.
         else
         {
            reg.DATA = data;

            // Send data, wait for data interrupt.
            reg.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
         }
      }
      else
      {
         // Enable stop interrupt.
         uint8_t currentCtrlA = reg.CTRLA;
         reg.CTRLA = currentCtrlA | TWI_SLAVE_PIEN_bm;

         // Fetch data
         data = reg.DATA;

         // If application signalling need to abort (error occured),
         // complete transaction and wait for next START. Otherwise
         // send ACK and wait for data interrupt.
         if ( finish )
         {
            reg.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
            DEBUG_L1( 'n' );
         }
         else
         {
            reg.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
            DEBUG_L1( 'a' );
         }
      }
   }
   // If unexpected state.
   else
   {
      ERROR_1( "S.ERR_PROTOCOL" );
      return ERR_PROTOCOL;
   }
   return STATUS_OK;
}

uint16_t Twi::Slave::read( void* pData, uint16_t length )
{
   IStream::TransferDescriptor transferDescriptor;

   // Set the transfer descriptor
   transferDescriptor.pData = static_cast<uint8_t*>( pData );
   transferDescriptor.bytesRemaining = length;
   transferDescriptor.bytesTransferred = 0;

   waitTransferCompleted( transferDescriptor );
   return transferDescriptor.bytesTransferred;
}
