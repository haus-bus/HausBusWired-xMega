/*
 * LogicalButton.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_LogicalButton_H
#define HwUnits_LogicalButton_H

#include "HwUnits.h"
#include <ConfigurationManager.h>

class Button;

class CriticalSection;

class Event;

class HACF;

class Led;

class Scheduler;

class evMessage;

class LogicalButton : public Reactive
{
   public:

      class Configuration
      {
         public:

            static const uint8_t MAX_OBJECTS = 8;

            ////    Attributes    ////

            uint8_t buttonId[MAX_OBJECTS];

            uint8_t ledId[MAX_OBJECTS];

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

            uint8_tx buttonId[Configuration::MAX_OBJECTS];

            uint8_tx ledId[Configuration::MAX_OBJECTS];

            inline uint8_t getLedId( uint8_t idx )
            {
               if ( idx < Configuration::MAX_OBJECTS )
               {
                  return ledId[idx];
               }
               return 0xFF;
            }

            inline uint8_t getButtonId( uint8_t idx )
            {
               if ( idx < Configuration::MAX_OBJECTS )
               {
                  return buttonId[idx];
               }
               return 0xFF;
            }
      };

      class Command
      {
         public:

            enum Commands
            {
               GET_CONFIGURATION = HACF::COMMANDS_START,
               SET_CONFIGURATION,
               OFF,
               ON,
               BLINK,
               GET_STATUS,
               SET_MIN_BRIGHTNESS,
               RESERVED_2,
               RESERVED_3,
               RESERVED_4,
               RESERVED_5,
               SET_BUTTON_CONFIGURATION,
               SET_LED_CONFIGURATION
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
               STATUS,

            };

            union Parameter
            {
               Configuration configuration;
               uint8_t status;
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

      class ErrorCode
      {
         public:

            enum Errors
            {
               NO_LED,
               NO_BUTTON,
            };
      };

      ////    Constructors and destructors    ////

      LogicalButton( uint8_t _id );

      ////    Operations    ////

      void clearObjectList();

      uint8_t getLedStatus();

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size );

      void routeMessageToButton( const Event& event );

      void routeMessageToLed( const Event& event );

      void updateObjectList();

   protected:

      bool handleRequest( const Event& event );

      ////    Additional operations    ////

   public:

      void addButton( Button* p_Button );

      inline void setConfiguration( EepromConfiguration* _config )
      {
         configuration = _config;
      }

      void addLed( Led* p_Led );

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      ////    Attributes    ////

      static const uint8_t debugLevel;

      ////    Relations and components    ////

      Button* button[Configuration::MAX_OBJECTS];

      EepromConfiguration* configuration;

      Led* led[Configuration::MAX_OBJECTS];

};

inline void* LogicalButton::operator new( size_t size )
{
   return allocOnce( size );
}

#endif
