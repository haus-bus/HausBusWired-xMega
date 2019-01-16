/*
 * Crc7.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Security_Crc7_H
#define SwFramework_Security_Crc7_H

#include "Security.h"
#include <DefaultTypes.h>

class Crc7
{
   ////    Operations    ////

   public:

      inline static uint8_t hasError( void* pData, uint16_t length )
      {
         return get( pData, length );
      }

      static uint8_t get( void* pData, uint16_t length, uint8_t crc = 0 );
};


#endif
