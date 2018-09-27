/*
 * ConfigurationManager.h
 *
 * Created: 21.09.2018 14:12:56
 * Author: viktor.pankraz
 */

#ifndef __CONFIGURATIONMANAGER_H__
#define __CONFIGURATIONMANAGER_H__

#include <xEeprom.h>
#include <Scheduler.h>


class ConfigurationManager
{
   public:

      class EepromConfigurationBase
      {

         static const uint8_t MIN_CONFIGURATION_SIZE = 8;
         static const uint8_t MAX_CONFIGURATION_SIZE = 32;
         static const uint16_t FREE_CONFIGURATION_ID = 0xFFFF;

         public:

            static inline EepromConfigurationBase* getFirstConfiguration()
            {
               return reinterpret_cast<EepromConfigurationBase*>( MAPPED_EEPROM_START + 32 );
            }

            static inline EepromConfigurationBase* getLastConfiguration()
            {
               return reinterpret_cast<EepromConfigurationBase*>( MAPPED_EEPROM_END - 24 );
            }

            inline bool isMine( uint16_t _id, uint8_t size )
            {
               return ( ownerId == _id ) && ( reservedSize == size );
            }

            inline bool isFree()
            {
               return ownerId == FREE_CONFIGURATION_ID;
            }

            inline bool isValidOwner()
            {
               return Scheduler::getJob( ownerId );
            }

            inline bool isValidSize()
            {
               return ( reservedSize >= MIN_CONFIGURATION_SIZE )
                      && ( reservedSize <= MAX_CONFIGURATION_SIZE );
            }

            inline uint8_t getReservedSize()
            {
               return reservedSize;
            }

            inline void setReservedSize( uint16_t _size )
            {
               reservedSize = _size;
            }

            inline void setOwnerId( uint16_t _id )
            {
               ownerId = _id;
            }

            inline EepromConfigurationBase* getNextConfiguration()
            {
               return (EepromConfigurationBase*)( ( (uint16_t)this ) + reservedSize );
            }


         protected:

            uint16_tx ownerId;

            uint8_tx reservedSize;
      };

      template<typename T>
      class EepromConfigurationTmpl : public EepromConfigurationBase
      {
         public:
            inline void get( T& conf )
            {
               Eeprom::read( ( (uint16_t)this ) + sizeof( EepromConfigurationBase ), &conf, sizeof( conf ) );
            }

            inline void set( T& conf )
            {
               conf.checkAndCorrect();
               Eeprom::write( ( (uint16_t)this ) + sizeof( EepromConfigurationBase ), &conf, sizeof( conf ) );
            }

            inline void restore()
            {
               T conf = T::getDefault();
               set( conf );
            }
      };

// variables
   public:
   protected:
   private:
      static const uint8_t debugLevel;

// functions
   public:

      template<class T>
      static inline T* getConfiguration( uint16_t id )
      {
         T* conf = (T*)findConfiguration( id, sizeof( T ) );
         if ( conf != NULL )
         {
            if ( conf->isFree() )
            {
               conf->setOwnerId( id );
               conf->setReservedSize( sizeof( T ) );
               conf->restore();
            }
         }
         return conf;
      }

   protected:

      static EepromConfigurationBase* findConfiguration( uint16_t id, uint8_t size );

   private:

}; // ConfigurationManager

#endif // __CONFIGURATIONMANAGER_H__
