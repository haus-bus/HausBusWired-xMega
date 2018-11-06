/*
 * MS6System.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */
#ifndef Systems_HomeAutomationSystems_MS6System_H
#define Systems_HomeAutomationSystems_MS6System_H

#include <DigitalPort.h>
#include <HomeAutomation.h>

#include "PbsSystemHw.h"

class PbsSystem : public HomeAutomation
{
   ////    Constructors and destructors    ////

   public:

      PbsSystem();

      ////    Operations    ////

      static void start();

      ////    Additional operations    ////

      ////    Relations and components    ////

   protected:

      DigitalPort digitalPortA;

      DigitalPort digitalPortB;

      DigitalPort digitalPortC;

      DigitalPort digitalPortD;

      PbsSystemHw hardware;
};

#endif
