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
#include "IStream.h"

class EventDrivenUnit;

class IoStream : public IStream
{
   public:

      enum Command
      {
         INIT,
         IS_BUSY,
         STOP,
		 FLUSH,
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

      virtual IStream::Status genericCommand( Command command, void* buffer );

      virtual IStream::Status read( void* pData, uint16_t length, EventDrivenUnit* user = 0 );

      virtual IStream::Status write( void* pData, uint16_t length, EventDrivenUnit* user = 0 );
};

#endif

