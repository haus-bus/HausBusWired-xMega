/*
 * ModuleId.h
 *
 * Created: 18.06.2014 14:12:56
 * Author: viktor.pankraz
 */

#ifndef SwFramework_Security_ModuleId_H
#define SwFramework_Security_ModuleId_H

#include "Security.h"
#include <DefaultTypes.h>

class ModuleId
{
   ////    Additional operations    ////

   public:

      inline char getName( int i1 ) const
      {
         return name[i1];
      }

      inline void setName( int i1, char p_name )
      {
         name[i1] = p_name;
      }

      inline uint32_t getSize() const
      {
         return size;
      }

      inline void setSize( uint32_t p_size )
      {
         size = p_size;
      }

      inline uint8_t getMajorRelease() const
      {
         return majorRelease;
      }

      inline void setMajorRelease( uint8_t p_majorRelease )
      {
         majorRelease = p_majorRelease;
      }

      inline uint8_t getMinorRelease() const
      {
         return minorRelease;
      }

      inline void setMinorRelease( uint8_t p_minorRelease )
      {
         minorRelease = p_minorRelease;
      }

      inline uint8_t getFirmwareId() const
      {
         return firmwareId;
      }

      inline void setFirmwareId( uint8_t p_firmwareId )
      {
         firmwareId = p_firmwareId;
      }

      inline uint8_t getChecksum() const
      {
         return checksum;
      }

      inline void setChecksum( uint8_t p_checksum )
      {
         checksum = p_checksum;
      }

      ////    Attributes    ////

      char name[16];

      uint32_t size;

      uint8_t majorRelease;

      uint8_t minorRelease;

      uint8_t firmwareId;

      uint8_t checksum;
};

#endif

