/*
 * ApplicationTable.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_UserSignature_H
#define Basics_UserSignature_H

#include "Basics.h"

#include "Peripherals/Flash.h"

class UserSignature
{
   public:

      ////    Operations    ////

      inline static uint8_t read( uint16_t offset )
      {
         return Flash::readUserSignature( offset );
      }

      inline static uint16_t read( uint16_t offset, void* pData, uint16_t length )
      {
         return Flash::readUserSignature( offset, pData, length );
      }

      inline static uint16_t write( uint16_t offset, void* pData, uint16_t length )
      {
         return Flash::writeUserSignature( offset, pData, length );
      }
};


#endif

