/*
 * InternalEeprom.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_InternalEeprom_H
#define Basics_InternalEeprom_H

#include "InternalEeprom.h"

#include <StorageInterface.h>

class Eeprom;

class InternalEeprom : public StorageInterface
{
   ////    Constructors and destructors    ////

   public:

      inline InternalEeprom()
      {
      }

      ////    Operations    ////

      virtual uint16_t read( uint16_t offset, void* pData, uint16_t length );

      virtual uint16_t write( uint16_t offset, void* pData, uint16_t length );
};

#endif

