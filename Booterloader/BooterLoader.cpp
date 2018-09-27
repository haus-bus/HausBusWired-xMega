/*
 * BooterLoader.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "BooterLoader.h"

#include <Release.h>

const ModuleId moduleId =
{
   "$MOD$ " MOD_ID,
   0,
   Release::MAJOR,
   Release::MINOR,
   CONTROLLER_ID,
   0
};

int main( int argc, char* argv[] )
{
   int status = 0;
   {
      BooterLoaderHw hardware;
      hardware.copyBooter();
      ResetSystem::reset();
      while ( 1 )
      {
      }
   }
   return status;
}

