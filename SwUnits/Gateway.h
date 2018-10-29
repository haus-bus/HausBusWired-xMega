/*
 * Gateway.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Gateway_H
#define Gateway_H

#include "SwUnits.h"
#include "Basics.h"

#include <IoStream.h>
#include <Reactive.h>
#include <Protocols/HACF.h>
#include <Containers/StaticQueue.h>
#include <Time/Timestamp.h>
#include <ConfigurationManager.h>

class IResponse;

class Scheduler;

class Gateway : public Reactive
{
   public:

      static const uint8_t MAX_GATEWAYS = 4;
      static const uint8_t MESSAGE_QUEUE_SIZE = 64;
      static const uint8_t MAX_RETRIES = 5;
      static const uint8_t RETRY_DELAY_TIME = 50;
      static const uint8_t MIN_IDLE_TIME = 3;
      static const uint16_t MAGIC_NUMBER = 0xEFEF;

      typedef StaticQueue<HACF*, uint8_t, MESSAGE_QUEUE_SIZE> MessageQueue;

      enum Errorcodes
      {
         NO_ERRORS,
         WRITE_FAILED,
         READ_FAILED,
         MSG_QUEUE_OVERRUN,
         BUSY_BUS,
         RESET_BUS,
         BUFFER_OVERRUN,
         NO_CONFIGURATION
      };

      enum Instances
      {
         TWI = 1,
         SNMP = 2,
         MODBUS = 3,
         RS485 = 4,

         UDP = 15,
         UNKNOWN = 0xFF

      };

      struct Header
      {
         uint8_t address;
         uint8_t checksum;
         uint16_t lastDeviceId;
      };

      struct ReadFailedData
      {
         uint8_t checksum;
         uint16_t transferred;
         uint16_t remaining;
      };

      struct Configuration
      {
         static const uint8_t DEFAULT_OPTIONS = 0x01; // enabled
         static const uint8_t MAX_OPTIONS_MASK = 1;

         struct Options
         {
            uint8_t enabled : 1;
            uint8_t reserved : 7;
         };

         union Option
         {
            uint8_t mask;
            Options options;
         };

         ////    Attributes    ////

         Option option;



         ////    Operations    ////

         static inline Configuration getDefault()
         {
            Configuration defaultConfiguration =
            {
               .option = { DEFAULT_OPTIONS }
            };
            return defaultConfiguration;
         }

         inline void checkAndCorrect()
         {
            if ( option.mask > MAX_OPTIONS_MASK )
            {
               option.mask = DEFAULT_OPTIONS;
            }
         }

      };

      class EepromConfiguration : public ConfigurationManager::EepromConfigurationTmpl<Configuration>
      {
         public:

            XEeprom<Configuration::Option> option;

            uint32_tx reserve;

            inline Configuration::Options getOptions() const
            {
               return option.operator*().options;
            }
      };

      class Command
      {
         public:

            enum Commands
            {
               GET_CONFIGURATION = HACF::COMMANDS_START,
               SET_CONFIGURATION
            };

            union Parameter
            {
               Configuration setConfiguration;
            };

            ////    Operations    ////

            inline Parameter& getParameter()
            {
               return parameter;
            }

            ////    Additional operations    ////

            inline uint8_t getCommand() const
            {
               return command;
            }

            inline void setCommand( uint8_t p_command )
            {
               command = p_command;
            }

            inline void setParameter( Parameter p_parameter )
            {
               parameter = p_parameter;
            }

            ////    Attributes    ////

            uint8_t command;

            Parameter parameter;
      };

      class Response : public IResponse
      {
         public:

            enum Responses
            {
               CONFIGURATION = HACF::RESULTS_START,

               EVENT_GATEWAY_LOAD = HACF::EVENTS_START,
            };

            struct GatewayLoad
            {
               uint16_t messagesPerMinute;

               uint32_t bytesPerMinute;
            };

            union Parameter
            {
               Configuration configuration;
               GatewayLoad gatewayLoad;
            };

            ////    Constructors and destructors    ////

            inline Response( uint16_t id ) : IResponse( id )
            {
            }

            inline Response( uint16_t id, const HACF& message ) : IResponse( id, message )
            {
            }

            ////    Operations    ////

            inline Parameter& getParameter()
            {
               return *reinterpret_cast<Parameter*>( IResponse::getParameter() );
            }

            void setGatewayLoad( GatewayLoad& gatewayLoad );

            Parameter& setConfiguration();

            ////    Attributes    ////

         private:

            Parameter params;
      };

      ////    Constructors and destructors    ////

      inline Gateway( IoStream* _ioStream = NULL, Instances iId = UNKNOWN ) : packetCounter( 0 ), ioStream( _ioStream ), configuration( NULL )
      {
         addGateway();
         gatewayLoad.bytesPerMinute = 0;
         gatewayLoad.messagesPerMinute = 0;
         setId( ( ClassId::GATEWAY << 8 ) | iId );
      }

      ////    Operations    ////

   public:

      void terminate();

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size )
      {
         return allocOnce( size );
      }

      inline static Gateway* getItsGateway( uint8_t idx )
      {
         return itsGateway[idx];
      }

      inline static void setListener( EventDrivenUnit* _listener )
      {
         listener = _listener;
      }

      inline void setConfiguration( EepromConfiguration* _config )
      {
         configuration = _config;
      }

   protected:

      void run();

      void notifyEndOfWriteTransfer( Stream::Status status );

      void notifyEndOfReadTransfer( Stream::TransferDescriptor* td );

      void addGateway();

      uint8_t getDebugLevel();

      uint8_t getNumOfGateways();

      void notifyError( uint8_t errorCode, uint8_t* errorData = 0 );

      void notifyMessageReceived( HACF::ControlFrame* controlFrame );

      void notifyMessageSent( HACF* hacf );

      void reportGatewayLoad();

      void setNumOfGateways( uint8_t p_numOfGateways );

      bool handleRequest( HACF* message );

      ////    Attributes    ////

      uint8_t packetCounter;

      Timestamp lastReportTimestamp;

      Response::GatewayLoad gatewayLoad;

      static uint8_t numOfGateways;

      ////    Relations and components    ////

      static Gateway* itsGateway[MAX_GATEWAYS];

      static EventDrivenUnit* listener;

      Timestamp lastIdleTime;

      Stream::Status writeStatus[MAX_RETRIES];

      uint8_t retries;

      IoStream* ioStream;

      ReadFailedData readFailedData;

      MessageQueue itsMessageQueue;

      EepromConfiguration* configuration;
};

#endif
