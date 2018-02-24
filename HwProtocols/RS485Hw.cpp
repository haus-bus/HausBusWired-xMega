/*
 * RS485Hw.cpp
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */

#include <RS485Hw.h>
#include <EventPkg/EventPkg.h>

const uint8_t RS485Hw::debugLevel( DEBUG_LEVEL_OFF );

void RS485Hw::handleDataReceived()
{
  uint8_t data;
  bool isAddressFrame = usart.isAddressFrame();
  usart.read( data );

  if ( isAddressFrame )
  {
    if ( data == 0xAA )
    {
      receiveBufferSize = 0;
      usart.disableMpcm();
    }
    else
    {
      // message completed
      usart.enableMpcm();
      if ( transmitBuffer )
      {
        disableTransmitter();
        transmitBuffer = NULL;
      }
      else if( owner )
      {
        Stream::TransferDescriptor td;
        td.bytesTransferred = receiveBufferSize;
        td.pData = receiveBuffer;
        evData( owner, &td ).send();
      }
    }
  }
  else
  {
    if ( transmitBuffer )
    {
      if ( transmitBuffer[receiveBufferSize] != data )
      {
        // notify collision
        disableTransmitter();
        usart.enableTransmitter();
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
}

bool RS485Hw::init()
{
  bool hasError = usart.init<100000, true, USART_CMODE_ASYNCHRONOUS_gc,
      USART_PMODE_DISABLED_gc, USART_CHSIZE_9BIT_gc, false>();
  if ( !hasError )
  {
    usart.enableMpcm();
    disableReceiver();
    disableTransmitter();
    disableRx.configOutput();
    enableTx.configOutput();
  }
  return hasError;
}

Stream::Status RS485Hw::genericCommand( IoStream::Command command,
                                        void* buffer )
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

    bool error = init();
    if ( error )
    {
      return ABORTED;
    }
    owner = cmd->owner;
    maxReceiveBufferSize = cmd->buffersize;
    usart.enableMpcm();
    usart.enableReceiveCompleteInterrupt();
    enableReceiver();
  }
  return SUCCESS;
}

Stream::Status RS485Hw::read( void * pData, uint16_t length,
                              EventDrivenUnit* user )
{
  return STOPPED;
}

Stream::Status RS485Hw::write( void * pData, uint16_t length,
                               EventDrivenUnit* user )
{
  usart.enableTransmitter();
  enableTransmitter();

  // Start frame
  usart.setAdressFrame();
  usart.write( 0xAA );

  // data frames
  usart.clearAdressFrame();
  transmitBuffer = (uint8_t*) pData;
  usart.write( transmitBuffer, length );
  usart.waitUntilTransferCompleted();

  // Stop frame
  usart.setAdressFrame();
  usart.write( getInstanceId() );
  usart.waitUntilTransferCompleted();

  if ( !usart.isTransmitterEnabled() || memcmp( receiveBuffer, pData, length ) )
  {
    disableTransmitter();
    return ABORTED;
  }
  return SUCCESS;
}

