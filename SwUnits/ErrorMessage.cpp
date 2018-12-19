/*
 * ErrorMessage.cpp
 *
 *  Created on: 23.10.2015
 *      Author: viktor.pankraz
 */

#include <ErrorMessage.h>

ErrorMessage::ErrorMessage( uint16_t id, uint8_t code, uint8_t* data ) :
   IResponse( id )
{
   setErrorCode( code, data );
   queue();
}

void ErrorMessage::notifyOutOfMemory( uint16_t _id )
{
   ErrorMessage( _id, CONFIGURATION_OUT_OF_MEMORY );
}

