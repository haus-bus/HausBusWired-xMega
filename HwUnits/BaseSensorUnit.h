/*
 * BaseSensorUnit.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_BaseSensorUnit_H
#define HwUnits_BaseSensorUnit_H

#include "HwUnits.h"
#include <ConfigurationManager.h>

class CriticalSection;

class HACF;

class Scheduler;

class evMessage;

class BaseSensorUnit : public Reactive
{
   public:

      struct Status
      {
         int8_t value;
         uint8_t centiValue;
         uint8_t lastEvent;
      };

      static const uint8_t MAX_ERRORS = 10;

      class Configuration
      {
         public:

            static const uint8_t DEFAULT_LOWER_THRESOLD = 18;
            static const uint8_t DEFAULT_UPPER_THRESOLD = 22;
            static const uint8_t DEFAULT_REPORT_TIME = 60;
            static const uint8_t DEFAULT_HYSTERESIS = 5;

            ////    Attributes    ////

            int8_t lowerThreshold;

            int8_t upperThreshold;

            uint8_t reportTime;

            uint8_t hysteresis;

            ////    Operations    ////

            static inline Configuration getDefault()
            {
               Configuration defaultConfiguration =
               {
                  .lowerThreshold = DEFAULT_LOWER_THRESOLD,
                  .upperThreshold = DEFAULT_UPPER_THRESOLD,
                  .reportTime = DEFAULT_REPORT_TIME,
                  .hysteresis = DEFAULT_HYSTERESIS,
               };
               return defaultConfiguration;
            }

            inline void checkAndCorrect()
            {
               if ( lowerThreshold > upperThreshold )
               {
                  lowerThreshold = upperThreshold;
               }
            }
      };

      class EepromConfiguration : public ConfigurationManager::EepromConfigurationTmpl<Configuration>
      {
         public:

            int8_tx lowerThreshold;

            int8_tx upperThreshold;

            uint8_tx reportTime;

            uint8_tx hysteresis;
      };

      class Command
      {
         public:

            enum Commands
            {
               GET_CONFIGURATION = HACF::COMMANDS_START,
               SET_CONFIGURATION,
               GET_STATUS,
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

               EVENT_BELOW = HACF::EVENTS_START,
               EVENT_IN_RANGE,
               EVENT_ABOVE,
            };

            union Parameter
            {
               Configuration configuration;
               Status status;
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

            inline void setEvent( uint8_t value )
            {
               controlFrame.setDataLength( 1 );
               setResponse( value );
            }

            Parameter& setConfiguration();

            void setStatus( Status value );

            ////    Attributes    ////

         private:
            Parameter params;

      };

      ////    Constructors and destructors    ////

      inline BaseSensorUnit() :
         timeToReport( 1 ), currentEvent( 0 ), lastEvent( 0 ), errorCounter( 0 )
      {
         configuration = NULL;
      }

      ////    Operations    ////

      void notifyNewValue( Status newStatus );

   protected:

      bool handleRequest( HACF* message );

      ////    Additional operations    ////

   public:

      inline void setConfiguration( EepromConfiguration* _config )
      {
         configuration = _config;
      }

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      inline Status getLastStatus() const
      {
         return lastStatus;
      }

      inline void setLastStatus( Status p_lastStatus )
      {
         lastStatus = p_lastStatus;
      }

      inline uint8_t getTimeToReport() const
      {
         return timeToReport;
      }

      inline void setTimeToReport( uint8_t p_timeToReport )
      {
         timeToReport = p_timeToReport;
      }

      ////    Attributes    ////

      static const uint8_t debugLevel;

      Status lastStatus;

      uint8_t timeToReport;

      uint8_t currentEvent;

      uint8_t lastEvent;

      uint8_t errorCounter;

      ////    Relations and components    ////

      EepromConfiguration* configuration;

};

#endif
