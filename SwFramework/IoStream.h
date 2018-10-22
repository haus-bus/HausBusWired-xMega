/*
 * IoStream.h
 *
 *  Created on: 24.06.2014
 *      Author: viktor.pankraz
 */

#ifndef SwFramework_IoStream_H
#define SwFramework_IoStream_H

#include "SwFramework.h"
#include "DefaultTypes.h"
#include "Stream.h"

class EventDrivenUnit;

class IoStream : public Stream
{
   public:

      enum Command
      {
         INIT,
         IS_BUSY,
         STOP,
         RESTART
      };

      class CommandINIT
      {
         public:
            uint8_t deviceId;
            uint16_t buffersize;
            EventDrivenUnit* owner;
      };

      ////    Operations    ////

      virtual Stream::Status genericCommand( Command command, void* buffer );

      virtual Stream::Status read( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

      virtual Stream::Status write( void* pData, uint16_t length, EventDrivenUnit* user = 0 );
};

#endif

