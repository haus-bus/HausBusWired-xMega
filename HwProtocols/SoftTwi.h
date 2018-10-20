/*
 * SoftTwi.h
 *
 *  Created on: 28.12.2016
 *      Author: viktor.pankraz
 */

#ifndef HWPROTOCOLS_SOFTTWI_H_
#define HWPROTOCOLS_SOFTTWI_H_

#include <IoStream.h>

class SoftTwi : public IoStream
{
   public:

      ////    Constructors and destructors    ////

      inline SoftTwi()
      {
      }

      ////    Operations    ////

   public:

      virtual Stream::Status genericCommand( Command command, void* buffer );

      virtual Stream::Status read( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

      virtual Stream::Status write( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

      ////    Attributes    ////

   protected:

};


#endif /* HWPROTOCOLS_SOFTTWI_H_ */
