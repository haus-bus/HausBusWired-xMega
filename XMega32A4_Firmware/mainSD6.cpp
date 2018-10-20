/*
 * main.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */
#include "PbsSystem.h"

int main( int argc, char* argv[] )
{
   int status = 0;
   {
      PbsSystem::start();
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

#ifndef _DEBUG_
   WatchDog::reset();
#else
   PORTR.OUTCLR = Pin0;
#endif

}
