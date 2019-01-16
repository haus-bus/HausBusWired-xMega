/*
 * IoStream.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "IoStream.h"
#include "EventDrivenUnit.h"

IStream::Status IoStream::genericCommand( Command command, void* buffer )
{
   return STOPPED;
}

IStream::Status IoStream::read( void* pData, uint16_t length,
                                EventDrivenUnit* user )
{
   return STOPPED;
}

IStream::Status IoStream::write( void* pData, uint16_t length,
                                 EventDrivenUnit* user )
{
   return STOPPED;
}

