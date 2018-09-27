/*
 * RollerShutter.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_RollerShutter_H
#define HwUnits_RollerShutter_H

#include "HwUnits.h"
#include <ConfigurationManager.h>
#include <RollerShutterHw.h>

class CriticalSection;

class Event;

class HACF;

class Scheduler;

class evMessage;

class RollerShutter : public Reactive
{
   public:

      class Command;

      class Response;

      enum SubStates
      {
         NO_COMMAND,
         START_MOTOR,
         MOTOR_IS_RUNNING,
         STOP_MOTOR,
         NOTIFY_RUNNING,
         NOTIFY_STOPPED
      };

      enum Direction
      {
         TO_OPEN = -1,
         TOGGLE = 0,
         TO_CLOSE = 1
      };

      struct Configuration
      {
         static const uint16_t TIME_FACTOR = 1000;

         static const uint8_t DEFAULT_OPTIONS = 0;
         static const uint8_t DEFAULT_POSITION = 0;
         static const uint8_t DEFAULT_CLOSE_TIME = 20;
         static const uint8_t DEFAULT_OPEN_TIME = 21;
         static const uint8_t MAX_LEVEL = 100;
         static const uint8_t CLOSED_THRESHOLD = 95;
         static const uint8_t OPEN_THRESHOLD = 5;
         static const uint16_t DEFAULT_MOTOR_START_DELAY = 256;

         struct Options
         {
            uint8_t inverted : 1;
         };

         union Option
         {
            uint8_t mask;
            Options bit;
         };

         ////    Attributes    ////

         uint8_t closeTime;

         uint8_t openTime;

         Option option;


         ////    Functions    ////

         static inline Configuration getDefault()
         {
            Configuration defaultConfiguration =
            {
               .closeTime = DEFAULT_CLOSE_TIME,
               .openTime = DEFAULT_OPEN_TIME,
               .option = { DEFAULT_OPTIONS }
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

            uint8_tx closeTime;

            uint8_tx openTime;

            XEeprom<Configuration::Option> option;

            uint8_tx reserve;

            uint8_tx position;

            inline Configuration::Options getOptions() const
            {
               return option.operator*().bit;
            }
      };

      class Command
      {
         public:

            enum Commands
            {
               GET_CONFIGURATION = HACF::COMMANDS_START,
               SET_CONFIGURATION,
               MOVE_TO_POSITION,
               START,
               STOP,
               GET_STATUS,
               SET_POSITION
            };

            union Parameter
            {
               uint8_t position;
               int8_t direction;
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

               EVENT_CLOSED = HACF::EVENTS_START,
               EVENT_START,
               EVENT_OPEN,
            };

            union Parameter
            {
               Configuration configuration;
               uint8_t position;
               int8_t direction;
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

            void setDirection( uint8_t direction );

            void setPosition( uint8_t position );

            void setStatus( uint8_t position );

            ////    Attributes    ////

         private:

            Parameter params;
      };

      ////    Constructors and destructors    ////

      RollerShutter( uint8_t _id, RollerShutterHw* _hardware );

      ////    Operations    ////

      void cmdMoveToPosition( uint8_t target );

      void cmdStop();

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size );

      void run();

   private:

      bool handleRequest( HACF* message );

      void handleRunningState();

      void setPosition( uint8_t p_position );

      void positionTick();

      ////    Additional operations    ////

   public:

      inline RollerShutterHw* getHardware() const
      {
         return hardware;
      }

      inline void setHardware( RollerShutterHw* p_RollerShutterHw )
      {
         hardware = p_RollerShutterHw;
      }

   private:

      inline uint8_t getPosition() const
      {
         return position;
      }

      inline static uint8_t getStartingChannels()
      {
         return startingChannels;
      }

      inline static void setStartingChannels( uint8_t p_startingChannels )
      {
         startingChannels = p_startingChannels;
      }

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      inline uint8_t getTargetPosition() const
      {
         return targetPosition;
      }

      inline void setTargetPosition( uint8_t p_targetPosition )
      {
         targetPosition = minimum<uint8_t>( p_targetPosition, Configuration::MAX_LEVEL );
      }

      inline uint16_t getPollTime() const
      {
         return pollTime;
      }

      inline void setPollTime( uint16_t p_pollTime )
      {
         pollTime = p_pollTime;
      }

      inline uint8_t getSubState() const
      {
         return subState;
      }

      inline void setSubState( uint8_t p_subState )
      {
         subState = p_subState;
      }

      inline uint8_t getToggleDirection() const
      {
         return toggleDirection;
      }

      inline void setToggleDirection( uint8_t p_toggleDirection )
      {
         toggleDirection = p_toggleDirection;
      }

      inline void setConfiguration( EepromConfiguration* p_RollerShutter )
      {
         configuration = p_RollerShutter;
      }

      ////    Attributes    ////

      static uint8_t startingChannels;

      static const uint8_t debugLevel;

      uint8_t subState;

      uint8_t position;

      uint8_t targetPosition;

      uint16_t pollTime;

      uint8_t toggleDirection;

      ////    Relations and components    ////

   protected:

      EepromConfiguration* configuration;

      RollerShutterHw* hardware;

};

inline void* RollerShutter::operator new( size_t size )
{
   return allocOnce( size );
}

#endif
