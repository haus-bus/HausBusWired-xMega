/*
 * IResponse.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef IResponse_H
#define IResponse_H

#include <Protocols/HACF.h>

class CriticalSection;

class Reactive;

class Scheduler;

class evMessage;

class IResponse : public HACF
{
   ////    Constructors and destructors    ////

   public:

      IResponse( uint16_t id );

      IResponse( uint16_t id, const HACF& request );

   protected:

      inline IResponse()
      {
      }

      ////    Operations    ////

   public:

      inline void* getParameter()
      {
         return &controlFrame.data[1];
      }

      inline uint8_t getResponse()
      {
         return controlFrame.data[0];
      }

      inline void setResponse( uint8_t response )
      {
         controlFrame.data[0] = response;
      }

      void queue( Reactive* reactive = 0 );

      void setErrorCode( uint8_t errorCode, uint8_t* errorData = 0 );

      void setup( uint16_t id );

      void setupForResult( const HACF& request );
};

#endif
