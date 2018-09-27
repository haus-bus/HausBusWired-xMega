/*
 * InternalEeprom.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "InternalEeprom.h"
#include "Peripherals/Eeprom.h"

uint16_t InternalEeprom::read( uint16_t offset, void* pData, uint16_t length )
{
   return Eeprom::read( offset, pData, length );
}

uint16_t InternalEeprom::write( uint16_t offset, void* pData, uint16_t length )
{
   return Eeprom::write( offset, pData, length );
}

