/*
 * Electronics.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include "Electronics.h"
#include <Peripherals/Flash.h>


uint8_t getFckE()
{
   uint8_t fcke = Flash::readUserSignature( 1 );
   if ( fcke == 0xFF )
   {
      return FCKE_V2_0;
   }
   return fcke;
}