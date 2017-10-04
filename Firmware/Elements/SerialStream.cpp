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

Stream::Status SerialStream::genericCommand( IoStream::Command command, void* buffer )
{
   if ( command == IoStream::FLUSH )
   {
      serial->waitUntilTransferCompleted();
      return Stream::SUCCESS;
   }
   return Stream::ABORTED;
}

Stream::Status SerialStream::read( uint8_t& data )
{

   return serial->read( data ) ? Stream::SUCCESS : Stream::STOPPED;
}

Stream::Status SerialStream::write( uint8_t data )
{
   return serial->write( data ) ? Stream::SUCCESS : Stream::ABORTED;
}

bool SerialStream::available()
{
   return serial->isReceiveCompleted();
}