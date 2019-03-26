/*
 * SystemConditions.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SystemConditions_H
#define SystemConditions_H

#include <Containers/Array.h>
#include <Containers/BitField.h>

class SystemConditions
{
   public:

      static const uint8_t MAX_ITEMS = 16;

      enum Types
      {
         BIT,
         BYTE,
         WORD,
         SBYTE,
         SWORD,

         EVENT = 0x80
      };

      static BitField<uint32_t> bitField;

      static Array<uint8_t, MAX_ITEMS> byteArray;

      static Array<uint16_t, MAX_ITEMS> wordArray;

};

#endif

