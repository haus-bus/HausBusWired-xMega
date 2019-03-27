/*
 * NvmController.cpp
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "NvmController.h"

#include <string.h>

void NvmController::executeCommand( NVM_CMD_t command )
{
   uint8_t oldCmd = NVM.CMD;
   NVM.CMD = command;
   CCPWrite( &NVM.CTRLA, NVM_CMDEX_bm );
   NVM.CMD = oldCmd;
}

uint16_t NvmController::readUserSignature( uint16_t index, void* pData, uint16_t length )
{
   if ( ( index + length ) > getPageSize() )
   {
      return 0;
   }

   uint8_t* destination = static_cast<uint8_t*>( pData );
   uint16_t len = length;
   while ( len-- )
   {
      *destination++ = readUserSignature( index++ );
   }
   return length;
}

uint16_t NvmController::writeUserSignature( uint16_t index, void* pData, uint16_t length )
{
   if ( ( index + length ) > getPageSize() )
   {
      return 0;
   }

   uint8_t buffer[getPageSize()];
   readUserSignature( 0, buffer, sizeof( buffer ) );
   memcpy( &buffer[index], pData, length );
   loadFlashPageBuffer( buffer );
   eraseUserSignature();
   waitWhileBusy();
   NVM_commonSPM( 0, NVM_CMD_WRITE_USER_SIG_ROW_gc );
   return length;
}

void NvmController::loadFlashPageBuffer( uint8_t* data )
{
   flushBuffer();
   for ( uint16_t i = 0; i < getPageSize(); i += 2 )
   {
      // Set up little-endian word.
      uint16_t w = *data++;
      w += ( *data++ ) << 8;
      NVM_loadWordToFlashBuffer( i, w );
   }

}

