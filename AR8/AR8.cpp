/*
 * AR8.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "AR8.h"
#include "AR8System.h"

int main( int argc, char* argv[] )
{
   int status = 0;
   {
      AR8System::start();
   }
   return status;
}

void notifyBusy()
{
   TRACE_PORT_SET( TR_IDLE_PIN );
   PORTR.OUTSET = Pin0;
}

void notifyIdle()
{
   TRACE_PORT_CLEAR( TR_IDLE_PIN );
   PORTR.OUTCLR = Pin0;

#ifndef _DEBUG_
   WatchDog::reset();
#endif

   // #]
}

void putc( char c )
{
   Usart::instance( PortD, 1 ).write( c );
}
