#include "Stream.h"

void Stream::TransferDescriptor::reset()
{
   setPData( 0 );
   setBytesTransferred( 0 );
   setBytesRemaining( 0 );
}

