/*
 * ApplicationTable.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Basics_ApplicationTable_H
#define Basics_ApplicationTable_H

#include "Basics.h"

#include "Peripherals/Flash.h"

class ApplicationTable
{
   public:

    #ifndef APPTABLE_SECTION_ADDR
    #define APPTABLE_SECTION_ADDR APPTABLE_SECTION_START
    #endif

      ////    Operations    ////

      inline static uint8_t read( uint16_t offset )
      {
         Flash::address_t address = APPTABLE_SECTION_ADDR + offset;
         return Flash::read( address );
      }

      inline static uint16_t read( uint16_t offset, void* pData, uint16_t length )
      {
         return Flash::read( APPTABLE_SECTION_ADDR + offset, pData, length );
      }

      inline static uint16_t write( uint16_t offset, void* pData, uint16_t length )
      {
         Flash::address_t address = APPTABLE_SECTION_ADDR + offset;
         return Flash::write( address, pData, length );
      }
};


#endif

