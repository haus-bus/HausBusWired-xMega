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

      enum Direction
      {
         To_CLOSE,
         TO_OPEN
      };

      class ErrorCode
      {
         public:

            enum Errors {
               NO_ERROR,
               INVALID_MOTOR_STATE,
            };
      };

      inline RollerShutterHw() : direction( false ), inverted( false ), independent( false )
      {

      }

      ////    Operations    ////

      bool isDirectionToClose();

      bool isDirectionToOpen();

      void off();

      void on();

      void setDirectionToClose();

      void setDirectionToOpen();

      inline void setInverted( bool _inverted )
      {
         inverted = _inverted;
         if ( !independent )
         {
            digitalOutput0.setInverted( inverted );
         }
      }

      inline void setIndependent( bool _independent )
      {
         independent = _independent;
      }

   private:

      uint8_t direction : 1;

      uint8_t inverted : 1;

      uint8_t independent : 1;
};
#endif
