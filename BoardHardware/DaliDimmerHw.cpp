/*
 * DaliDimmerHw.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include "DaliDimmerHw.h"

const uint8_t DaliDimmerHw::debugLevel( DEBUG_LEVEL_OFF );

DaliDimmerHw::DaliDimmerHw( uint8_t _address, Dali& _daliHw ) : address( _address ), daliHw( _daliHw )
{
}

uint8_t DaliDimmerHw::hasError()
{
   uint8_t cmd[2];
   cmd[0] = address | 1;
   cmd[1] = Dali::GET_STATUS;
   daliHw.write( cmd );
   Status status;
   daliHw.read( (uint8_t*)&status );

   return status.error;
}

uint16_t DaliDimmerHw::isOn()
{
   uint8_t cmd[2];
   cmd[0] = address | 1;
   cmd[1] = Dali::GET_CURR_VALUE;
   daliHw.write( cmd );
   uint8_t value;
   daliHw.read( &value );

   DEBUG_M4( FSTR( "addr " ), address, FSTR( " value " ), value );
   return value;
}

void DaliDimmerHw::on( uint8_t value )
{
   if ( value < 100 )
   {
      value = ( value << 1 ) + ( value >> 1 );
   }
   else
   {
      value = 254;
   }
   uint8_t cmd[2];
   cmd[0] = address;
   cmd[1] = value;
   daliHw.write( cmd );
   DEBUG_M4( FSTR( "addr " ), address, FSTR( " value " ), value );
}

uint8_t DaliDimmerHw::setMode( uint8_t mode )
{
   return 0;
}
