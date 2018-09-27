/*
 * InternalFlash.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "InternalFlash.h"
#include "Peripherals/Flash.h"

uint16_t InternalFlash::read( uint16_t offset, void* pData, uint16_t length )
{
   return Flash::read( offset, pData, length );
}

uint16_t InternalFlash::write( uint16_t offset, void* pData, uint16_t length )
{
   return Flash::write( offset, pData, length );
}

