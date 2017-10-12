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

      enum FIRMWARE_IDS
      {
         HMW_SD6_ID = 0xB1
      };

      enum HARDWARE_IDS
      {
         REV_0,
         REV_1
      };

      static const uint8_t MAJOR = 1;

      static const uint8_t MINOR = 1;
};

#endif
