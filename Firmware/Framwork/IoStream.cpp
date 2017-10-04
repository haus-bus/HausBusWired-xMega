/*
 * IoStream.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "IoStream.h"

Stream::Status IoStream::genericCommand( Command command, void* buffer )
{
   return STOPPED;
}

Stream::Status IoStream::read( uint8_t& data )
{
   return STOPPED;
}

Stream::Status IoStream::write( uint8_t data )
{
   return STOPPED;
}

bool IoStream::available()
{
   return false;
}

