/*
 * MonitoredInput.h
 *
 *  Created on: 09.05.2015
 *      Author: viktor.pankraz
 */

#ifndef HWPROTOCOLS_MONITOREDINPUTHW_H_
#define HWPROTOCOLS_MONITOREDINPUTHW_H_

#include "Protocols.h"
#include <PortPin.h>

class MonitoredInputHw
{

   public:

      inline MonitoredInputHw( PortPin pin ) :
         portPin( pin )
      {

      }

      uint16_t getResponseTime( uint16_t timeout = 500 );

      ////    Attributes    ////

   private:

      PortPin portPin;

};

#endif /* HWPROTOCOLS_MONITOREDINPUTHW_H_ */
