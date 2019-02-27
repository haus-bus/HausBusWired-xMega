/*
 * Release.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Release_H
#define Release_H

#include <stdint.h>

class Release
{
   public:

      enum FirmwareId
      {
         HBW_GENERIC = 0xB0,
         HBW_MultiKeySD6 = 0xB1,
         HBW_IO12_1W_UP = 0xB2,
         HBW_LC4_IN4_DR = 0xB3,

         HBW_LC_DIM8_1W_DR = 0xB8
      };

      enum HardwareId
      {
         REV_0,
         REV_1,
         REV_2

      };

      static const uint8_t MAJOR = 2;

      static const uint8_t MINOR = 7;
};

#endif
