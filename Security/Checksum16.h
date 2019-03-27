/*
 * Checksum16.h
 *
 * Created: 18.06.2014 14:12:56
 * Author: viktor.pankraz
 */

#ifndef SwFramework_Security_Checksum16_H
#define SwFramework_Security_Checksum16_H

#include "Security.h"
#include <DefaultTypes.h>

class Checksum16
{
   public:

      enum Constants {
         StartValue = 0
      };

      ////    Operations    ////

      static uint16_t get( void* pData, uint16_t length, uint32_t checksum = StartValue );

      inline static uint16_t hasError( void* pData, uint16_t length )
      {
         return get( pData, length ) + 1;
      }
};

#endif