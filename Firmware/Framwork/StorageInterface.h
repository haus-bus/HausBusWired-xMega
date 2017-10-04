/*
 * StorageInterface.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_StorageInterface_H
#define SwFramework_StorageInterface_H

#include "SwFramework.h"


class StorageInterface
{
   ////    Operations    ////

   public:

      // ## operation read(uint16_t,void *,uint16_t)
      virtual uint16_t read( uintptr_t offset, void* pData, uint16_t length ) = 0;

      // ## operation write(uint16_t,void *,uint16_t)
      virtual uint16_t write( uintptr_t offset, void* pData, uint16_t length ) = 0;
};

#endif

