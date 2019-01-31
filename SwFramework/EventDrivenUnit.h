/*
 * EventDrivenUnit.h
 *
 *  Created on: 24.06.2014
 *      Author: viktor.pankraz
 */

#ifndef SwFramework_EventDrivenUnit_H
#define SwFramework_EventDrivenUnit_H

#include "SwFramework.h"
#include "Object.h"
#include "EventPkg/EventPkg.h"


class EventDrivenUnit : public Object
{
   ////    Operations    ////

   public:

      virtual bool notifyEvent( const Event& event ) = 0;
};

#endif
