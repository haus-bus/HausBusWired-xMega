/*
 * SnmpAgent.h
 *
 *  Created on: 03.03.2016
 *      Author: viktor.pankraz
 */

#ifndef SNMPAGENT_H_
#define SNMPAGENT_H_

#include <Protocols/Snmp.h>
#include <Gateway.h>
#include <ConfigurationManager.h>
#include "HwUnits.h"

class SnmpAgent : public Gateway,
                  public Snmp
{
   public:

      enum NodeIndex
      {
         ISO,
         ORG,
         DOD,
         INTERNET,
         PRIVATE,
         ENTERPRISES,
         PANKRAZ,
         PRODUCTS,
         DEVICE_TYPE,
         DEVICE_ID,
         CLASS_ID,
         INSTANCE_ID,
         FUNCTION_ID,
         MAX_NODES
      };

      class Configuration
      {
         public:

            static const uint16_t DEFAULT_LOWER_THRESHOLD = 100;
            static const uint16_t DEFAULT_UPPER_THRESHOLD = 200;

            ////    Attributes    ////

            uint32_t trapDestinationIp;
            char communityString[17];

            ////    Operations    ////

            static inline Configuration getDefault()
            {
               Configuration defaultConfiguration =
               {
                  0xFFFFFFFF,
                  "public"
               };
               return defaultConfiguration;
            }

            inline void checkAndCorrect()
            {
            }
      };

      class EepromConfiguration : public ConfigurationManager::EepromConfigurationTmpl<Configuration>
      {
         public:

            uint32_tx trapDestinationIp;
            uint8_tx communityString[17];

            void getCommunityString( char* string ) const
            {
               memcpy( string, communityString, sizeof( communityString ) );
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
               CONFIGURATION = HACF::RESULTS_START
            };

            union Parameter
            {
               Configuration configuration;
            };

            ////    Constructors and destructors    ////

            inline Response( uint16_t id ) :
               IResponse( id )
            {
            }

            inline Response( uint16_t id, const HACF& message ) :
               IResponse( id, message )
            {
            }

            ////    Operations    ////

            inline Parameter& getParameter()
            {
               return *reinterpret_cast<Parameter*>( IResponse::getParameter() );
            }

            Parameter& setConfiguration();

            ////    Attributes    ////

         private:

            Parameter params;
      };

      ////    Constructors and destructors    ////

      SnmpAgent();

      ////    Operations    ////

      virtual bool notifyEvent( const Event& event );

      bool handleRequest( HACF* message );

      bool sendMessage( HACF* message );

      virtual uint8_t bindProc( uint8_t req_type, Snmp::ObjectIdentifier* oid,
                                Snmp::Sequence* out );

      inline void* operator new( size_t size );

      void run();

   protected:

      Reactive* getNextNode( HACF::Object& object );

      void updateConfiguration();

      bool isClassIdSupported( uint8_t classId )
      {
         return ( classId == ::Object::ClassId::BUTTON )
                || ( classId == ::Object::ClassId::DIGITAL_OUTPUT )
                || ( classId == ::Object::ClassId::SYSTEM );
      }

      uint8_t getFirstSupportedClassId()
      {
         return ::Object::ClassId::BUTTON;
      }

      inline void setConfiguration( EepromConfiguration* _config )
      {
         configuration = _config;
      }

      ////    Attributes    ////

   protected:

      static const uint8_t debugLevel;

      EepromConfiguration* configuration;

      ////    Relations and components    ////

};

inline void* SnmpAgent::operator new( size_t size )
{
   return allocOnce( size );
}

#endif /* SNMPAGENT_H_ */
