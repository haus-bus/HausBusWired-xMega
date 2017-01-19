/*
 * TwiHw.cpp
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */

#include <TwiHw.h>
#include <EventPkg/EventPkg.h>

const uint8_t TwiHw::debugLevel( DEBUG_LEVEL_LOW );

Stream::Status TwiHw::genericCommand( IoStream::Command command, void* buffer )
{
  if ( command == IoStream::INIT )
  {
    IoStream::CommandINIT* cmd = (IoStream::CommandINIT*) buffer;
    setInstanceId( cmd->deviceId );
    if ( receiveBuffer )
    {
      delete receiveBuffer;
    }
    receiveBuffer = 0;
    maxReceiveBufferSize = 0;

    receiveBuffer = new uint8_t[cmd->buffersize];
    if ( !receiveBuffer )
    {
      return INVALID_BUFFER;
    }
    owner = cmd->owner;
    maxReceiveBufferSize = cmd->buffersize;
    twi.init<true, 100000, TWI_MASTER_INTLVL_OFF_gc, TWI_SLAVE_INTLVL_LO_gc>();
    return SUCCESS;
  }
  return NOT_SUPPORTED;
}

Stream::Status TwiHw::read( void * pData, uint16_t length,
                            EventDrivenUnit* user )
{
  return STOPPED;
}

Stream::Status TwiHw::write( void * pData, uint16_t length,
                             EventDrivenUnit* user )
{
  if ( twi.master.write( Twi::DEFAULT_ADDRESS, pData, length, true ) != length )
  {
    return ABORTED;
  }
  return SUCCESS;
}

void TwiHw::notifyEndOfSlaveTransfer()
{
  if ( transmitBuffer )
  {
    // transfered data was received correct
    transmitBuffer = NULL;
  }
  else if ( owner )
  {
    Stream::TransferDescriptor td;
    td.bytesTransferred = receiveBufferSize;
    td.pData = receiveBuffer;
    evData( owner, &td ).send();
  }
  receiveBufferSize = 0;
}

void TwiHw::handleSlaveStatus()
{
  uint8_t data;
  uint8_t status = twi.slave.handleStatus(
      data, receiveBufferSize >= maxReceiveBufferSize );

  if ( status < Twi::STATUS_STOP )
  {
    if ( status == Twi::STATUS_START )
    {
      if ( receiveBufferSize )
      {
        // this happens if STOP was not seen
        WARN_1( "unexpected START received!" );
        notifyEndOfSlaveTransfer();
      }
      return;
    }
    DEBUG_L3( '/', data, '\\' );
    if ( transmitBuffer )
    {
      if ( transmitBuffer[receiveBufferSize] != data )
      {
        // notify collision
        transmitBuffer = NULL;
      }
    }
    receiveBuffer[receiveBufferSize] = data;
    if ( receiveBufferSize < maxReceiveBufferSize )
    {
      receiveBufferSize++;
    }
    else
    {
      // notify buffer overrun
    }
  }
  else
  {
    notifyEndOfSlaveTransfer();
  }
}
