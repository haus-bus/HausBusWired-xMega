/*
 * Event.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "EventDrivenUnit.h"

#include "EventPkg.h"

Event::MessageQueue Event::messageQueue;

bool Event::queue() const
{

   if ( destination )
   {
      return messageQueue.push( *this );
   }
   return false;

}

bool Event::send() const
{
   if ( destination )
   {
      return destination->notifyEvent( *this );
   }
   return false;
}

