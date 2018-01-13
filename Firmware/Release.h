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
         HMW_SD6 = 0xB1
      };

      enum HardwareId
      {
         REV_0,
         REV_1,
         REV_2

      };

      static const uint8_t MAJOR = 1;

      static const uint8_t MINOR = 13;
};

#endif
