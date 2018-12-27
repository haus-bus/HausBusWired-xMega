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
         SIM = 0,
         AR8_ID,
         MS6_ID,
         SD6_ID,
         SD485_ID,
         MAX_FW_ID
      };

      static const uint8_t MAJOR = 1;

      static const uint8_t MINOR = 12;
};

#endif
