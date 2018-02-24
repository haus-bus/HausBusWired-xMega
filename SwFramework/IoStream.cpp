#include "IoStream.h"
#include "EventDrivenUnit.h"

Stream::Status IoStream::genericCommand( Command command, void* buffer )
{
   return STOPPED;
}

Stream::Status IoStream::read( void* pData, uint16_t length,
                               EventDrivenUnit* user )
{
   return STOPPED;
}

Stream::Status IoStream::write( void* pData, uint16_t length,
                                EventDrivenUnit* user )
{
   return STOPPED;
}

