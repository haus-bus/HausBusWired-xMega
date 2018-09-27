/*
 * HomeAutomationHw.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_SystemBoards_HomeAutomationHw_H
#define Electronics_SystemBoards_HomeAutomationHw_H

#include "SystemBoards.h"

#include <ApplicationTable.h>
#include <GlobalInterrupt.h>
#include <Stream.h>

class Checksum;

class Eeprom;

class FlashString;

class Logger;

class ModuleId;

class HomeAutomationHw
{
   public:

      static const uint32_t BAUDRATE = 115200;

      ////    Constructors and destructors    ////

      inline HomeAutomationHw()
      {
      }

      ////    Operations    ////

      inline void enableInterrupts();

      inline static Flash::address_t findModuleIdPosition( bool loaderModId );

      inline static uint8_t getFirmwareId();

#ifdef _DEBUG_

      static FlashString* getId();

#endif

      static bool getModuleId( uint8_t index, ModuleId* moduleId );

      inline static uint16_t readRules( uint16_t offset, void* pData,
                                        uint16_t length );

      inline static uint16_t writeRules( uint16_t offset, void* pData,
                                         uint16_t length );

      ////    Additional operations    ////

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      ////    Attributes    ////

      static const uint8_t debugLevel;
};

inline void HomeAutomationHw::enableInterrupts()
{
   PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
   GlobalInterrupt::enable();
}

inline Flash::address_t HomeAutomationHw::findModuleIdPosition(
   bool loaderModId )
{
   Flash::address_t address;
   if ( loaderModId )
   {
      address = BOOT_SECTION_START + _VECTORS_SIZE;
   }
   else
   {
      address = APP_SECTION_START + _VECTORS_SIZE;
   }

   if ( Flash::read( address ) == '$' )
   {
      return address - 1;
   }
   return -1;
}

inline uint8_t HomeAutomationHw::getFirmwareId()
{
   return Flash::readUserSignature( 0 );
}

inline uint16_t HomeAutomationHw::readRules( uint16_t offset,
                                             void* pData,
                                             uint16_t length )
{
   return ApplicationTable::read( offset, pData, length );
}

inline uint16_t HomeAutomationHw::writeRules( uint16_t offset,
                                              void* pData,
                                              uint16_t length )
{
   if ( length )
   {
      return ApplicationTable::write( offset, pData, length );
   }
   return length;
}

#endif
