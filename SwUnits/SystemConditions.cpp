/*
 * SystemConditions.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "SystemConditions.h"

BitField<uint32_t> SystemConditions::bitField;

Array<uint8_t, SystemConditions::MAX_ITEMS> SystemConditions::byteArray;

Array<uint16_t, SystemConditions::MAX_ITEMS> SystemConditions::wordArray;
