/*
 * ConfigurationManager.cpp
 *
 * Created: 21.09.2018 14:12:55
 * Author: viktor.pankraz
 */


#include "ConfigurationManager.h"

const uint8_t ConfigurationManager::debugLevel( DEBUG_LEVEL_OFF );

ConfigurationManager::EepromConfigurationBase* ConfigurationManager::firstConfiguration( NULL );

ConfigurationManager::EepromConfigurationBase* ConfigurationManager::lastConfiguration( NULL );

ConfigurationManager::EepromConfigurationBase* ConfigurationManager::findConfiguration( uint16_t id, uint8_t size )
{
   EepromConfigurationBase* conf = EepromConfigurationBase::getFirstConfiguration();
   EepromConfigurationBase* lastConf = EepromConfigurationBase::getLastConfiguration();
   EepromConfigurationBase* unusedConf = NULL;

   while ( conf <= lastConf )
   {
      if ( conf->isMine( id, size ) )
      {
         DEBUG_M4( FSTR( "existing Conf in own slot " ), id, FSTR( "->" ), (uintptr_t)conf );
         return conf;
      }
      else if ( conf->isFree() )
      {
         DEBUG_M4( FSTR( "new Conf in free slot " ), id, FSTR( "->" ), (uintptr_t)conf );
         return conf;
      }
      else if ( !conf->isValidSize() )
      {
         DEBUG_M4( FSTR( "new Conf in invalid slot " ), id, FSTR( "->" ), (uintptr_t)conf );
         conf->setOwnerId( EepromConfigurationBase::FREE_CONFIGURATION_ID );
         return conf;
      }
      else if ( !conf->isValidOwner() && ( conf->getReservedSize() >= size ) )
      {
         unusedConf = conf;
      }
      conf = conf->getNextConfiguration();
   }
   if ( unusedConf )
   {
      DEBUG_M4( FSTR( "new Conf in unused slot " ), id, FSTR( "->" ), (uintptr_t)unusedConf );
      unusedConf->setOwnerId( EepromConfigurationBase::FREE_CONFIGURATION_ID );
   }
   return unusedConf;
}