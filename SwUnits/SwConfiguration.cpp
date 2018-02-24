/*
 * SwConfiguration.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "SwConfiguration.h"

#include <Security/Checksum.h>
#include <Scheduler.h>

SwConfiguration::SmallConfiguration* SwConfiguration::smallConfigurations(
   reinterpret_cast<SmallConfiguration*>( 32 ) );

StorageInterface* SwConfiguration::storage( 0 );

SwConfiguration::LargeConfiguration* SwConfiguration::largeConfigurations(
   reinterpret_cast<LargeConfiguration*>( 560 ) );

uint8_t SwConfiguration::maxSmallObjects( 64 );

uint8_t SwConfiguration::maxLargeObjects( 16 );

const uint8_t SwConfiguration::debugLevel( DEBUG_LEVEL_OFF );

void SwConfiguration::updateChecksum( uint16_t offset, uint8_t size )
{
   /*
      uint8_t buffer[size+2];
      storage->read( offset-2, buffer, sizeof(buffer) );
      uint8_t checksum = Checksum::get( buffer, size+2 );

      if( size < sizeof(SmallConfiguration) )
      {
      offset += ( sizeof(SmallConfiguration) - 3 );
      }
      else
      {
      offset += ( sizeof(LargeConfiguration) - 3 );
      }
      storage->write( offset, &checksum, sizeof(checksum) );
    */
   // #]
}

uint8_t SwConfiguration::getIndexOfConfiguration( uint16_t id, uint8_t size )
{
   uint8_t freeSlotIdx = 255, unusedSlotIdx = 255;
   uint8_t index;
   uintptr_t offset;
   LargeConfiguration conf;

   if ( size == sizeof( SmallConfiguration ) )
   {
      offset = (uintptr_t) smallConfigurations;
      index = maxSmallObjects;
   }
   else
   {
      offset = (uintptr_t) largeConfigurations;
      index = maxLargeObjects;
   }

   while ( index-- )
   {
      if ( storage->read( offset + index * size, &conf, size )
           != Stream::SUCCESS )
      {
         break;
      }
      if ( conf.owner == id )
      {
         DEBUG_M4( FSTR( "Conf " ), id, FSTR( "->" ), index );
         return index;
      }
      if ( ( freeSlotIdx == 255 ) && ( conf.owner == 0xFFFF ) )
      {
         freeSlotIdx = index;
      }
      if ( ( unusedSlotIdx == 255 ) && !Scheduler::getJob( conf.owner ) )
      {
         unusedSlotIdx = index;
      }
   }

   if ( freeSlotIdx != 255 )
   {
      storage->write( offset + freeSlotIdx * size, &id, sizeof( id ) );
      DEBUG_M4( FSTR( "new Conf in free slot " ), id, FSTR( "->" ), freeSlotIdx );
      return ( freeSlotIdx | NEW_INITIALIZED );
   }
   if ( unusedSlotIdx != 255 )
   {
      storage->write( offset + unusedSlotIdx * size, &id, sizeof( id ) );
      DEBUG_M4( FSTR( "new Conf in unused slot " ), id, FSTR( "->" ), unusedSlotIdx );
      return ( unusedSlotIdx | NEW_INITIALIZED );
   }

   ERROR_2( FSTR( "no Config found " ), id );
   return 0;
}
