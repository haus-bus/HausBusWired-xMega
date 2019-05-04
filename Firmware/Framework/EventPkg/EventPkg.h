/*
 * EventPkg.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_EventPkg_EventPkg_H
#define SwFramework_EventPkg_EventPkg_H

#include "IStream.h"
#include "DefaultTypes.h"
#include <Containers/StaticQueue.h>

class EventDrivenUnit;

class HBCP;

class evConnect;

class evCovered;

class evFree;

class evData;

class evEndOfTransfer;

class evWakeup;

class evMessage;

class evGatewayMessage;

class evTime;


enum EventIds
{
   idNullEvent,
   idEvStartup,
   idEvOn,
   idEvOff,
   idEvCovered,
   idEvFree,
   idEvStart,
   idEvStop,
   idEvCommand,
   idEvEvent,
   idEvTime,
   idEvEndOfTransfer,
   idEvRequest,
   idEvData,
   idEvMessage,
   idEvGatewayMessage,
   idEvWakeup,
   idEvTimeout,
   idEvUdpData,
   idEvDataRequest,
   idEvConnect
};

class Event
{

   static const uint8_t MAX_DATA_SIZE = 3;
   static const uint8_t MESSAGE_QUEUE_SIZE = 64;

   typedef StaticQueue<Event, uint8_t, MESSAGE_QUEUE_SIZE> MessageQueue;

   public:

      union Parameters
      {
         IStream::TransferDescriptor* td;
         HBCP* message;
         struct EndOfTransfer
         {
            IStream::TransferDescriptor* td;
            IStream::Status status;
         } endOfTransfer;
         bool status;
      };

      static MessageQueue messageQueue;

      ////    Constructors and destructors    ////

      inline Event( EventDrivenUnit* p_destination = 0 ) :
         destination( p_destination )
      {
         setId( idNullEvent );
      }

      ////    Operations    ////

      inline EventDrivenUnit* getDestination() const
      {
         return destination;
      }

      inline uint8_t getId() const
      {
         return id;
      }

      inline void setId( uint8_t p_id )
      {
         id = p_id;
      }

      inline evConnect* isEvConnect() const
      {
         if ( id != idEvConnect )
         {
            return NULL;
         }
         return (evConnect*) this;
      }

      inline evCovered* isEvCovered() const
      {
         if ( id != idEvCovered )
         {
            return NULL;
         }
         return (evCovered*) this;
      }

      inline evData* isEvData() const
      {
         if ( id != idEvData )
         {
            return NULL;
         }
         return (evData*) this;
      }

      inline evTime* isEvTime() const
      {
         if ( id != idEvTime )
         {
            return NULL;
         }
         return (evTime*) this;
      }

      inline evEndOfTransfer* isEvEndOfTransfer() const
      {
         if ( id != idEvEndOfTransfer )
         {
            return NULL;
         }
         return (evEndOfTransfer*) this;
      }

      inline evMessage* isEvMessage() const
      {
         if ( id != idEvMessage )
         {
            return NULL;
         }
         return (evMessage*) this;
      }

      inline evGatewayMessage* isEvGatewayMessage() const
      {
         if ( id != idEvGatewayMessage )
         {
            return NULL;
         }
         return (evGatewayMessage*) this;
      }

      inline evWakeup* isEvWakeup() const
      {
         if ( id != idEvWakeup )
         {
            return NULL;
         }
         return (evWakeup*) this;
      }

      bool queue() const;

      bool send() const;

      ////    Framework    ////
   protected:

      EventDrivenUnit* destination;
      uint8_t id;
      Parameters parameters;

};

class evCovered : public Event
{
   ////    Constructors and destructors    ////

   public:

      inline evCovered( EventDrivenUnit* p_destination = 0 ) :
         Event( p_destination )
      {
         setId( idEvCovered );
      }
};

class evData : public Event
{

   ////    Constructors and destructors    ////

   public:

      inline evData( EventDrivenUnit* p_destination = 0,
                     IStream::TransferDescriptor* p_transferDescriptor = 0 ) :
         Event( p_destination )
      {
         setId( idEvData );
         setTransferDescriptor( p_transferDescriptor );
      }

      inline void setTransferDescriptor(
         IStream::TransferDescriptor* p_transferDescriptor )
      {
         parameters.td = p_transferDescriptor;
      }

      inline IStream::TransferDescriptor* getTransferDescriptor()
      {
         return parameters.td;
      }

};

class evEndOfTransfer : public Event
{

   ////    Constructors and Destructors    ////

   public:

      inline evEndOfTransfer( EventDrivenUnit* p_destination = 0,
                              IStream::TransferDescriptor* p_transferDescriptor = 0,
                              IStream::Status p_status = IStream::ABORTED )
      {
         setId( idEvEndOfTransfer );
         setTransferDescriptor( p_transferDescriptor );
         setStatus( p_status );
      }

      inline void setTransferDescriptor(
         IStream::TransferDescriptor* p_transferDescriptor )
      {
         parameters.endOfTransfer.td = p_transferDescriptor;
      }

      inline IStream::TransferDescriptor* getTransferDescriptor()
      {
         return parameters.endOfTransfer.td;
      }

      inline void setStatus( IStream::Status status )
      {
         parameters.endOfTransfer.status = status;
      }

      inline IStream::Status getStatus()
      {
         return parameters.endOfTransfer.status;
      }
};

class evConnect : public Event
{

   struct Parameter
   {
      bool status;
   };

   ////    Constructors and destructors    ////

   public:

      inline evConnect( EventDrivenUnit* p_destination = 0, bool p_status = true ) :
         Event( p_destination )
      {
         setId( idEvConnect );
         setStatus( p_status );
      }

      inline void setStatus( bool status )
      {
         parameters.status = status;
      }

      inline bool getStatus() const
      {
         return parameters.status;
      }

};

class evWakeup : public Event
{
   ////    Constructors and destructors    ////

   public:

      inline evWakeup( EventDrivenUnit* p_destination = 0 ) :
         Event( p_destination )
      {
         setId( idEvWakeup );
      }
};

class evFree : public Event
{
   ////    Constructors and destructors    ////

   public:

      inline evFree( EventDrivenUnit* p_destination = 0 ) :
         Event( p_destination )
      {
         setId( idEvFree );
      }
};

class evMessage : public Event
{

   ////    Constructors and destructors    ////

   public:

      inline evMessage( EventDrivenUnit* p_destination = 0, HBCP* p_message = 0 ) :
         Event( p_destination )
      {
         setId( idEvMessage );
         setMessage( p_message );
      }

      inline void setMessage( HBCP* message )
      {
         parameters.message = message;
      }

      inline HBCP* getMessage()
      {
         return parameters.message;
      }

};

class evGatewayMessage : public evMessage
{
   ////    Constructors and destructors    ////

   public:

      inline evGatewayMessage( EventDrivenUnit* p_destination = 0, HBCP* p_message
                                  = 0 ) :
         evMessage( p_destination, p_message )
      {
         setId( idEvGatewayMessage );
      }

};

class evTime : public Event
{
   ////    Constructors and destructors    ////

   public:

      inline evTime( EventDrivenUnit* p_destination = 0 ) :
         Event( p_destination )
      {
         setId( idEvTime );
      }
};

#endif
