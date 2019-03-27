/*
 * ModBusTcp.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "ModBusTcp.h"
#include "EventPkg/EventPkg.h"
#include "Protocols/IpStack/IpConnection.h"
#include "IStream.h"

const uint8_t ModBusTcp::debugLevel( DEBUG_LEVEL_OFF );

uint16_t ModBusTcp::Adu::Header::getLength() const
{
   return changeEndianness( length );
}

uint16_t ModBusTcp::Adu::Header::getProtocol() const
{
   return changeEndianness( protocol );
}

uint16_t ModBusTcp::Adu::Header::getTransaction() const
{
   return changeEndianness( transaction );
}

void ModBusTcp::Adu::Header::setLength( uint16_t p_length )
{
   length = changeEndianness( p_length );
}

void ModBusTcp::Adu::Header::setProtocol( uint16_t p_protocol )
{
   protocol = changeEndianness( p_protocol );
}

void ModBusTcp::Adu::Header::setTransaction( uint16_t p_transaction )
{
   transaction = changeEndianness( p_transaction );
}

void ModBusTcp::Adu::setException( const ModBusTcp::Adu::Exception exception )
{
   functionCode |= FUNCTION_ERROR;
   data[0] = exception;
   header.setLength( 3 );
}

uint16_t* ModBusTcp::ReadRegister::changeIntoResponse()
{
   address = calculateResponseDataLength();
   return reinterpret_cast<uint16_t*>( ( (uint8_t*) this ) + 1 );
}

uint16_t ModBusTcp::ReadRegister::getAddress() const
{
   return changeEndianness( address );
}

uint16_t ModBusTcp::ReadRegister::getLength() const
{
   return changeEndianness( length );
}

uint16_t ModBusTcp::ReadStatus::calculateResponseDataLength()
{
   uint16_t len = getLength();
   if ( len % 8 )
   {
      len += 8;
   }
   return ( len / 8 );
}

uint8_t* ModBusTcp::ReadStatus::changeIntoResponse()
{
   address = calculateResponseDataLength();
   return reinterpret_cast<uint8_t*>( this ) + 1;
}

uint16_t ModBusTcp::ReadStatus::getAddress() const
{
   return changeEndianness( address );
}

uint16_t ModBusTcp::ReadStatus::getLength() const
{
   return changeEndianness( length );
}

uint16_t ModBusTcp::WriteRegister::getAddress() const
{
   return changeEndianness( address );
}

uint16_t ModBusTcp::WriteRegister::getValue() const
{
   return changeEndianness( value );
}

uint16_t ModBusTcp::WriteMultipleRegister::getAddress() const
{
   return changeEndianness( address );
}

uint16_t ModBusTcp::WriteMultipleRegister::getLength() const
{
   return changeEndianness( length );
}

uint16_t ModBusTcp::WriteMultipleRegister::getData( uint8_t idx ) const
{
   return changeEndianness( data[idx] );
}

uint16_t ModBusTcp::WriteMultipleCoils::getAddress() const
{
   return changeEndianness( address );
}

uint16_t ModBusTcp::WriteMultipleCoils::getLength() const
{
   return changeEndianness( length );
}

ModBusTcp::ModBusTcp()
{
}

bool ModBusTcp::notifyEvent( const Event& event )
{
   if ( event.isEvConnect() )
   {
      const evConnect* ev = event.isEvConnect();
      if ( ev->getStatus() )
      {
         IpConnection::unlisten( SERVER_PORT );
      }
      else
      {
         IpConnection::listen( SERVER_PORT, ev->getDestination() );
      }
      DEBUG_M1( ( ev->getStatus() ? FSTR( "connected" ) : FSTR( "disconnected" ) ) );
   }
   else if ( event.isEvData() )
   {
      IStream::TransferDescriptor* td = event.isEvData()->getTransferDescriptor();

      if ( td->bytesTransferred != 0 )
      {
         // we received new data
         DEBUG_M1( FSTR( "command received" ) );
         Adu* adu = (Adu*) td->pData;
         Adu::Exception exception = Adu::EX_NONE;
         bool input = true;
         switch ( adu->getFunctionCode() )
         {
            case Adu::READ_HOLDING_REGISTER:
               input = false;
            case Adu::READ_INPUT_REGISTER:
            {
               ReadRegister* cmd = (ReadRegister*) adu->data;
               DEBUG_M4( FSTR( "readHoldingRegister(" ), cmd->getAddress(), ',',
                         cmd->getLength() );
               DEBUG_L1( ')' );
               uint16_t responseLength = cmd->calculateResponseLength();
               exception = Adu::EX_ILLEGAL_DATA_VALUE;
               if ( responseLength <= ( sizeof( Adu ) - sizeof( adu->header ) ) )
               {
                  adu->header.setLength( responseLength );
                  exception = readRegister( cmd, input );
               }
               break;
            }
            case Adu::READ_COILS:
               input = false;
            case Adu::READ_DISCRETE_INPUTS:
            {
               ReadStatus* cmd = (ReadStatus*) adu->data;
               DEBUG_M3( FSTR( "read" ), ( input ? FSTR( "Input" ) : FSTR( "Coil" ) ),
                         FSTR( "Status(" ) );
               DEBUG_L4( cmd->getAddress(), ',', cmd->getLength(), ')' );
               uint16_t responseLength = cmd->calculateResponseLength();
               exception = Adu::EX_ILLEGAL_DATA_VALUE;
               if ( responseLength <= ( sizeof( Adu ) - sizeof( adu->header ) ) )
               {
                  adu->header.setLength( responseLength );
                  exception = readStatus( cmd, input );
               }
               break;
            }

            case Adu::WRITE_SINGLE_COIL:
            case Adu::WRITE_REGISTER:
            {
               exception = writeRegister( (WriteRegister*) adu->data );
               break;
            }

            case Adu::WRITE_MULTIPLE_COILS:
            {
               WriteMultipleCoils* cmd = (WriteMultipleCoils*) adu->data;
               exception = writeMultipleCoils( cmd );
               cmd->changeIntoResponse();
               adu->header.setLength( adu->header.getLength() - 1 );
               break;
            }

            case Adu::WRITE_MULTIPLE_REGISTERS:
            {
               WriteMultipleRegister* cmd = (WriteMultipleRegister*) adu->data;
               exception = writeMultipleRegister( cmd );
               cmd->changeIntoResponse();
               adu->header.setLength( adu->header.getLength() - 1 );
               break;
            }

            case Adu::DIAG_DIAGNOSTIC:
            {
               // do nothing, the message will be returned unmodified
               break;
            }
            default:
            {
               DEBUG_M3( FSTR( "functionCode: 0x" ), adu->getFunctionCode(),
                         FSTR( " not supported" ) );
               exception = Adu::EX_ILLEGAL_FUNCTION;
               break;
            }
         }
         if ( exception != Adu::EX_NONE )
         {
            adu->setException( exception );
         }
         td->bytesTransferred = adu->header.getLength() + sizeof( adu->header ) - 1;
      }
      else
      {
         // if we want to send anything, we can do it now
         td->bytesTransferred = 0;
      }
   }
   else if ( event.isEvEndOfTransfer() )
   {
      DEBUG_M1( FSTR( "last data acked" ) );
      IStream::TransferDescriptor* td = event.isEvEndOfTransfer()->getTransferDescriptor();
      // if we want to send anything, we can do it now
      td->bytesTransferred = 0;
   }
   return true;
}

ModBusTcp::Adu::Exception ModBusTcp::readRegister( ModBusTcp::ReadRegister* cmd,
                                                   bool inputsOnly )
{
   return Adu::EX_ILLEGAL_FUNCTION;
}

ModBusTcp::Adu::Exception ModBusTcp::readStatus( ModBusTcp::ReadStatus* cmd,
                                                 bool inputsOnly )
{
   return Adu::EX_ILLEGAL_FUNCTION;
}

ModBusTcp::Adu::Exception ModBusTcp::writeRegister(
   ModBusTcp::WriteRegister* cmd )
{
   return Adu::EX_ILLEGAL_FUNCTION;
}

ModBusTcp::Adu::Exception ModBusTcp::writeMultipleRegister(
   ModBusTcp::WriteMultipleRegister* cmd )
{
   return Adu::EX_ILLEGAL_FUNCTION;
}

ModBusTcp::Adu::Exception ModBusTcp::writeMultipleCoils(
   ModBusTcp::WriteMultipleCoils* cmd )
{
   return Adu::EX_ILLEGAL_FUNCTION;
}
