/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: AR8
        Configuration   : release
        Model Element	: DaliDimmerHw
   //!	Generated Date	: Wed, 6, Aug 2014
        File Path	: AR8/release/Electronics/HwUnitBoards/DaliDimmerHw.cpp
 *********************************************************************/

// ## auto_generated
#include "DaliDimmerHw.h"
// ## package Electronics::HwUnitBoards

// ## class DaliDimmerHw
const uint8_t DaliDimmerHw::debugLevel( DEBUG_LEVEL_OFF );

DaliDimmerHw::DaliDimmerHw( uint8_t _address, Dali& _daliHw ) : address( _address ), daliHw( _daliHw )
{
   // #[ operation DaliDimmerHw(uint8_t,Dali)
   // #]
}

uint8_t DaliDimmerHw::hasError()
{
   // #[ operation hasError()
   uint8_t cmd[2];
   cmd[0] = address | 1;
   cmd[1] = Dali::GET_STATUS;
   daliHw.write( cmd );
   Status status;
   daliHw.read( (uint8_t*)&status );

   return status.error;
   // #]
}

uint16_t DaliDimmerHw::isOn()
{
   // #[ operation isOn()
   uint8_t cmd[2];
   cmd[0] = address | 1;
   cmd[1] = Dali::GET_CURR_VALUE;
   daliHw.write( cmd );
   uint8_t value;
   daliHw.read( &value );

   DEBUG_M4( FSTR( "addr " ), address, FSTR( " value " ), value );
   return value;
   // #]
}

void DaliDimmerHw::on( uint8_t value )
{
   // #[ operation on(uint8_t)
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
   // #]
}

uint8_t DaliDimmerHw::setMode( uint8_t mode )
{
   // #[ operation setMode(uint8_t)
   return 0;
   // #]
}

/*********************************************************************
        File Path	: AR8/release/Electronics/HwUnitBoards/DaliDimmerHw.cpp
*********************************************************************/
