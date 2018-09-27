/*
 * RollerShutterHw.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_HwUnitBoards_RollerShutterHw_H
#define Electronics_HwUnitBoards_RollerShutterHw_H

#include "HwUnitBoards.h"
#include "SlotHw.h"
class Logger;


class RollerShutterHw : public SlotHw
{
   public:

      class ErrorCode
      {
         public:

            enum Errors {
               NO_ERROR,
               INVALID_MOTOR_STATE,
            };
      };

      ////    Operations    ////

      bool isDirectionToClose();

      bool isDirectionToOpen();

      void off();

      void on();

      void setDirectionToClose();

      void setDirectionToOpen();

      inline void setInverted( bool inverted );
};

inline void RollerShutterHw::setInverted( bool inverted )
{
   digitalOutput0.setInverted( inverted );
}

#endif
