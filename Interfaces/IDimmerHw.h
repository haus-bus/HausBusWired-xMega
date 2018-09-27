/*
 * IDimmerHw.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef IDimmerHw_H
#define IDimmerHw_H

#include <stdint.h>

class IDimmerHw
{
   public:

      class ErrorCode;

      static const uint8_t MAX_BRIGHTNESS = 100;

      enum SuspendModes
      {
         ENABLE,
         DISABLE,
         CHECK_ENABLED
      };

      class ErrorCode
      {
         public:

            enum Errors
            {
               NO_FAILTURE,
               NO_ZERO_CROSS_DETECTED,
               INVALID_PERIOD,
               INVALID_MODE,
               DEFECT,
            };
      };

      ////    Operations    ////

      virtual uint8_t hasError() = 0;

      virtual uint16_t isOn() = 0;

      virtual void on( uint16_t value ) = 0;

      virtual uint8_t setMode( uint8_t mode ) = 0;

      virtual uint8_t suspend( uint8_t mode );

      inline void off()
      {
         on( 0 );
      }
};

#endif
