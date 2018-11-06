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
   if ( independent )
   {
      return direction == To_CLOSE;
   }
   return digitalOutput0.isSet();
}

bool RollerShutterHw::isDirectionToOpen()
{
   if ( independent )
   {
      return direction == TO_OPEN;
   }
   return !digitalOutput0.isSet();
}

void RollerShutterHw::off()
{
   digitalOutput1.clear();

   if ( independent )
   {
      digitalOutput0.clear();
      return;
   }
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
   if ( independent )
   {
      if ( direction ^ inverted )
      {
         digitalOutput0.set();
      }
      else
      {
         digitalOutput1.set();
      }
      return;
   }
   digitalOutput1.set();
   _delay_ms( POWER_SWITCH_DELAY );
}

void RollerShutterHw::setDirectionToClose()
{
   if ( independent )
   {
      direction = To_CLOSE;
   }
   else
   {
      digitalOutput0.set();
   }
}

void RollerShutterHw::setDirectionToOpen()
{
   if ( independent )
   {
      direction = TO_OPEN;
   }
   else
   {
      digitalOutput0.clear();
   }
}
