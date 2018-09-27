/*
 * DigitalPort.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_DigitalPort_H
#define HwUnits_DigitalPort_H

#include "HwUnits.h"
#include "Button.h"
#include "Counter.h"
#include "Led.h"
#include <ConfigurationManager.h>
#include <DigitalPortHw.h>

class CriticalSection;

class DS1820;

class Dht;

class Event;

class HACF;

class IrReceiver;

class OneWire;

class PortPinUnit;

class Scheduler;

class evMessage;

class string;

class DigitalPort : public Reactive
{
   public:


      union PinFunction
      {
         Object* object;
         Counter* counter;
         Button* button;
         Led* led;
      };

      enum ErrorCodes
      {
         PINFUNCTION_NOT_SUPPORTED = 0x10
      };

      class Configuration
      {
         public:

            static const uint8_t MAX_PINS = 8;

            static const uint8_t RefreshTime = 10;

            ////    Attributes    ////

            uint8_t pinFunction[MAX_PINS];

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

            uint8_tx pinFunction[Configuration::MAX_PINS];

            inline uint8_t getPinFunction( uint8_t idx )
            {
               if ( idx < Configuration::MAX_PINS )
               {
                  return pinFunction[idx];
               }
               return 0xFF;
            }

            inline void setPinFunction( uint8_t idx, uint8_t _pinFunction )
            {
               if ( idx < Configuration::MAX_PINS )
               {
                  pinFunction[idx] = _pinFunction;
               }
            }
      };

      class Command
      {
         public:

            enum Commands
            {
               GET_CONFIGURATION = HACF::COMMANDS_START,
               SET_CONFIGURATION,
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

      inline DigitalPort( uint8_t portNumber ) :
         notUseablePins( 0 ), state( 0 ), counter0( 0xFF ), counter1( 0xFF ),
         hardware( portNumber )
      {
         configuration = NULL;
         clearPinFunction();
         setId( ( ClassId::DIGITAL_PORT << 8 ) | ( ( portNumber + 1 ) << 4 ) );
      }

      ////    Operations    ////

      inline uint8_t isPinUsable( uint8_t pinMask );

      virtual bool notifyEvent( const Event& event );

      void run();

      void updateLeds();

   protected:

      bool handleRequest( HACF* message );

   private:

      void clearPinFunction();

      void configureHw();

      uint8_t debounce();

      void notifyButtonChanges( uint8_t changedPins );

      ////    Additional operations    ////

   public:

      inline uint8_t getNotUseablePins() const
      {
         return notUseablePins;
      }

      inline void setNotUseablePins( uint8_t p_notUseablePins )
      {
         notUseablePins = p_notUseablePins;
      }

      inline uint8_t getState() const
      {
         return state;
      }

      inline void setState( uint8_t p_state )
      {
         state = p_state;
      }

      inline void setConfiguration( EepromConfiguration*  _config )
      {
         configuration = _config;
      }


   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

   private:

      ////    Attributes    ////

   public:

      uint8_t notUseablePins;

      uint8_t state;

   protected:

      static const uint8_t debugLevel;

   private:

      uint8_t counter0;

      uint8_t counter1;

      PinFunction pin[Configuration::MAX_PINS];

      ////    Relations and components    ////

   protected:

      EepromConfiguration* configuration;

      DigitalPortHw hardware;

};

inline uint8_t DigitalPort::isPinUsable( uint8_t pinMask )
{
   return !( pinMask & notUseablePins );
}

#endif
