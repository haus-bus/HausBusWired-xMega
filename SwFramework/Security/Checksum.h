/*
 * Checksum.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Security_Checksum_H
#define SwFramework_Security_Checksum_H

#include "Security/Security.h"
#include "DefaultTypes.h"

class Checksum
{
public:

  enum Constants
  {
    StartValue = 1
  };

  ////    Operations    ////

  static uint8_t get( void * pData, uint16_t length, uint8_t checksum =
                          StartValue );

  inline static uint8_t hasError( void * pData, uint16_t length )
  {
    return get( pData, length );
  }
};

#endif
