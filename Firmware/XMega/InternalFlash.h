/*
 * InternalFlash.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_InternalFlash_H
#define Basics_InternalFlash_H

#include <StorageInterface.h>

class InternalFlash : public StorageInterface
{
   ////    Operations    ////

   public:

      virtual uint16_t read( uint16_t offset, void* pData, uint16_t length );

      virtual uint16_t write( uint16_t offset, void* pData, uint16_t length );
};

#endif

