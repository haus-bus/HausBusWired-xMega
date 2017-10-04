/*
 * IoStream.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_IoStream_H
#define SwFramework_IoStream_H

#include "Stream.h"


class IoStream : public Stream
{
   public:

      enum Command
      {
         INIT,
         IS_BUSY,
         STOP,
         RESTART,
         FLUSH,
      };

      ////    Operations    ////

      virtual Stream::Status genericCommand( Command command, void* data = NULL );

      virtual Stream::Status read( uint8_t& data );

      virtual Stream::Status write( uint8_t data );

      virtual bool available();

};

#endif

