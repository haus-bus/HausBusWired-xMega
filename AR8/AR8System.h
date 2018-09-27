/*
 * AR8System.h
 *
 *  Created on: 06.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Systems_HomeAutomationSystems_AR8System_H
#define Systems_HomeAutomationSystems_AR8System_H

#include "AR8SystemHw.h"

#include <DigitalPort.h>
#include <HomeAutomation.h>


class AR8System : public HomeAutomation
{
   ////    Constructors and destructors    ////

   public:

      AR8System();

      ////    Operations    ////

      static void start();

      ////    Additional operations    ////

      DigitalPort* getDigitalPortE() const;

      DigitalPort* getDigitalPortF() const;

      AR8SystemHw* getHardware() const;

      ////    Relations and components    ////

   protected:

      DigitalPort digitalPortE;

      DigitalPort digitalPortF;

      AR8SystemHw hardware;
};

#endif
