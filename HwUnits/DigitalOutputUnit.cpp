/*
 * DigitalOutputUnit.cpp
 *
 *  Created on: 03.07.2018
 *      Author: Viktor Pankraz
 */

#include "DigitalOutputUnit.h"
#include <PortPin.h>


bool DigitalOutputUnit::notifyEvent( const Event& event )
{
   Event checkedEvent = event;

   if ( event.isEvMessage() )
   {
      HACF::ControlFrame& cf = checkedEvent.isEvMessage()->getMessage()->controlFrame;
      Command* data = reinterpret_cast<Command*>( cf.getData() );
      uint8_t maxOnTime = configuration->dutyOffset;
      uint8_t offDelayTime = configuration->minDuty;

      if ( cf.isCommand( Command::OFF ) && ( state.sub == ON ) && offDelayTime )
      {
         cf.setCommand( Command::ON );
         data->parameter.on.duration = offDelayTime;
      }
      else if ( cf.isCommand( Command::ON ) )
      {
         if ( maxOnTime )
         {
            if ( ( data->parameter.on.duration == 0 ) || ( data->parameter.on.duration > maxOnTime ) )
            {
               data->parameter.on.duration = maxOnTime;
            }
         }
      }
      else if ( cf.isCommand( Command::TOGGLE ) )
      {
         if ( maxOnTime )
         {
            if ( ( data->parameter.toggle.onTime == 0 ) || ( data->parameter.toggle.onTime > maxOnTime ) )
            {
               data->parameter.toggle.onTime = maxOnTime;
            }
         }
         if ( offDelayTime )
         {
            if ( data->parameter.toggle.onTime < offDelayTime )
            {
               data->parameter.toggle.onTime = offDelayTime;
            }
         }
      }
   }

   return PortPinUnit::notifyEvent( checkedEvent );
}

