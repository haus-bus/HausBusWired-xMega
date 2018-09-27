/*
 * ConfigurationManager.cpp
 *
 * Created: 21.09.2018 14:12:55
 * Author: viktor.pankraz
 */


#include "ConfigurationManager.h"

const uint8_t ConfigurationManager::debugLevel( DEBUG_LEVEL_LOW );

ConfigurationManager::EepromConfigurationBase* ConfigurationManager::findConfiguration( uint16_t id, uint8_t size )
{
   EepromConfigurationBase* conf = EepromConfigurationBase::getFirstConfiguration();
   EepromConfigurationBase* lastConf = EepromConfigurationBase::getLastConfiguration();

   while ( conf < lastConf )
   {
      if ( conf->isMine( id, size ) )
      {
         DEBUG_M4( FSTR( "new Conf in own slot " ), id, FSTR( "->" ), (uint16_t)conf );
         return conf;
      }
      else if ( conf->isFree() )
      {
         DEBUG_M4( FSTR( "new Conf in free slot " ), id, FSTR( "->" ), (uint16_t)conf );
         return conf;
      }
      else if ( !conf->isValidOwner() && ( !conf->isValidSize() || conf->getReservedSize() == size ) )
      {
         DEBUG_M4( FSTR( "new Conf in invalid/unused slot " ), id, FSTR( "->" ), (uint16_t)conf );
         return conf;
      }
      conf = conf->getNextConfiguration();
   }
   return NULL;
}