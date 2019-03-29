/*
 * SerialStream.cpp
 *
 * Created: 21.04.2017 00:18:42
 * Author: Viktor Pankraz
 */

#include "SerialStream.h"


SerialStream::SerialStream( Usart* _serial, PortPin _rx, PortPin _tx )
{
   serial = _serial;
   _tx.configOutput();
   _rx.configInput();
}

IStream::Status SerialStream::genericCommand( IoStream::Command command, void* buffer )
{
   if ( command == IoStream::FLUSH )
   {
      serial->waitUntilTransferCompleted();
      return IStream::SUCCESS;
   }
   return IStream::ABORTED;
}

IStream::Status SerialStream::read( uint8_t& data )
{

   return serial->read( data ) ? IStream::SUCCESS : IStream::STOPPED;
}

IStream::Status SerialStream::write( uint8_t data )
{
   return serial->write( data ) ? IStream::SUCCESS : IStream::ABORTED;
}

bool SerialStream::available()
{
   return serial->isReceiveCompleted();
}