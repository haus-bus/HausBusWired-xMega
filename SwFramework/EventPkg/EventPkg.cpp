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

