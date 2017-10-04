/*
 * Stream.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Stream.h"

void Stream::TransferDescriptor::reset()
{
   setPData( 0 );
   setBytesTransferred( 0 );
   setBytesRemaining( 0 );
}

