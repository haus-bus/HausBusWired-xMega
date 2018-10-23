/*
 * PbsSystemHw.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef PbsSystemHw_H
#define PbsSystemHw_H

#include <HomeAutomationHw.h>
#include <InternalEeprom.h>

class PbsSystemHw : public HomeAutomationHw
{
   ////    Constructors and destructors    ////

   public:

      PbsSystemHw();

      ////    Operations    ////

      void configure();

      void configureLogicalButtons();

      void configureTwi();

      void configureRs485();

      ////    Additional operations    ////


      ////    Attributes    ////


      ////    Relations and components    ////

   protected:

};

#endif
