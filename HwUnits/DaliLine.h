/*
 * DaliLine.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_DaliLine_H
#define HwUnits_DaliLine_H

#include "HwUnits.h"
#include <ConfigurationManager.h>
#include <Dali.h>


class CriticalSection;

class Event;

class HACF;

class Scheduler;

class evMessage;

class DaliLine : public Reactive
{
   public:

      static const uint16_t SLEEP_TIME = SystemTime::S;

      class Configuration
      {
         public:

            ////    Attributes    ////

            uint32_t dummy1;
            uint32_t dummy2;

            ////    Operations    ////

            static inline Configuration getDefault()
            {
               Configuration defaultConfiguration;
               memset( &defaultConfiguration, 0xFF, sizeof( defaultConfiguration ) );
               return defaultConfiguration;
            }

            inline void checkAndCorrect()
            {
            }
      };

      class EepromConfiguration : public ConfigurationManager::EepromConfigurationTmpl<Configuration>
      {
         public:

            uint32_tx dummy1;
            uint32_tx dummy2;
      };

      class Command
      {
         public:

            enum Commands
            {
               GET_CONFIGURATION = HACF::COMMANDS_START,
               SET_CONFIGURATION,
               ALL_OFF,
               ALL_ON,
               SEND_CMD
            };

            struct SendCmd
            {
               uint8_t command;
               uint8_t address;
            };

            union Parameter
            {
               SendCmd sendCmd;
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
               STATUS,

               EVENT_ERROR = HACF::EVENTS_END
            };

            union Parameter
            {
               uint8_t status;
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

            void setStatus( uint8_t status );

            ////    Attributes    ////

         private:

            Parameter params;

      };

      ////    Constructors and destructors    ////

      DaliLine( Dali& _daliHw, uint8_t _id = 1 );

      ////    Operations    ////

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size );

      void run();

   private:

      bool handleRequest( HACF* message );

      ////    Additional operations    ////

   public:

      inline void setConfiguration( EepromConfiguration* _config )
      {
         configuration = _config;
      }

   private:

      inline Dali& getDaliHw() const
      {
         return *daliHw;
      }

      inline void setDaliHw( Dali* p_daliHw )
      {
         daliHw = p_daliHw;
      }

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      ////    Attributes    ////

      Dali* daliHw;

      static const uint8_t debugLevel;

      ////    Relations and components    ////

   protected:

      EepromConfiguration* configuration;

};

inline void* DaliLine::operator new( size_t size )
{
   return allocOnce( size );
}

#endif
