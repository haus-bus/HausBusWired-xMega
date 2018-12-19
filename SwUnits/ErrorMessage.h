/*
 * ErrorMessage.h
 *
 *  Created on: 23.10.2015
 *      Author: viktor.pankraz
 */

#ifndef ERRORMESSAGE_H_
#define ERRORMESSAGE_H_

#include "IResponse.h"

class ErrorMessage : public IResponse
{

   public:

      enum GlobalErrorCode
      {
         CONFIGURATION_OUT_OF_MEMORY = 0x80,
         HEAP_OUT_OF_MEMORY,
      };

      ErrorMessage( uint16_t id, uint8_t code, uint8_t* data = 0 );

      static void notifyOutOfMemory( uint16_t _id );
};

#endif /* ERRORMESSAGE_H_ */
