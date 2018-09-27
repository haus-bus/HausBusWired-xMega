/*
 * RollerShutterHw.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */


#include "RollerShutterHw.h"
#include <Traces/Logger.h>


bool RollerShutterHw::isDirectionToClose()
{
   return digitalOutput0.isSet();
}

bool RollerShutterHw::isDirectionToOpen()
{
   return !digitalOutput0.isSet();
}

void RollerShutterHw::off()
{
   digitalOutput1.clear();
   _delay_ms( POWER_SWITCH_DELAY );

   if ( digitalOutput0.isInverted() )
   {
      digitalOutput0.set();
   }
   else
   {
      digitalOutput0.clear();
   }
}

void RollerShutterHw::on()
{
   digitalOutput1.set();
   _delay_ms( POWER_SWITCH_DELAY );
}

void RollerShutterHw::setDirectionToClose()
{
   digitalOutput0.set();
}

void RollerShutterHw::setDirectionToOpen()
{
   digitalOutput0.clear();
}
