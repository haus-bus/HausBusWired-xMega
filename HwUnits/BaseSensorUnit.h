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
#include <ErrorMessage.h>

class CriticalSection;

class HACF;

class Scheduler;

class evMessage;

class BaseSensorUnit : public Reactive
{
   public:

      typedef uint8_t Hysteresis;

      struct Status
      {
         int8_t value;
         uint8_t centiValue;
         uint8_t lastEvent;

         int16_t getCompleteValue() const
         {
            return ( value * 100 + centiValue );
         }

         void setCompleteValue( int16_t _completeValue )
         {
            value = _completeValue / 100;
            centiValue = _completeValue % 100;
         }
      };

      static const uint8_t MAX_ERRORS = 10;

      class Configuration
      {
         public:

            static const uint8_t DEFAULT_LOWER_THRESOLD = 18;
            static const uint8_t DEFAULT_UPPER_THRESOLD = 22;
            static const uint8_t DEFAULT_THRESHOLD_FRACTION = 0;
            static const uint8_t MAX_THRESHOLD_FRACTION = 99;
            static const uint8_t MAX_REPORT_TIME_BASE = 60;     // seconds
            static const uint8_t DEFAULT_REPORT_TIME_BASE = 10; // seconds
            static const uint8_t DEFAULT_MIN_REPORT_TIME = 6;   // * ReportTimeBase
            static const uint8_t DEFAULT_MAX_REPORT_TIME = 60;  // * ReportTimeBase
            static const uint8_t DEFAULT_HYSTERESIS = 10;       // 0.1 * unit of threshold

            ////    Attributes    ////

            int8_t lowerThreshold;

            uint8_t lowerThresholdFraction;

            int8_t upperThreshold;

            uint8_t upperThresholdFraction;

            uint8_t reportTimeBase;

            uint8_t minReportTime;

            uint8_t maxReportTime;

            uint8_t hysteresis;

            ////    Operations    ////

            static inline Configuration getDefault()
            {
               Configuration defaultConfiguration =
               {
                  .lowerThreshold = DEFAULT_LOWER_THRESOLD,
                  .lowerThresholdFraction = DEFAULT_THRESHOLD_FRACTION,
                  .upperThreshold = DEFAULT_UPPER_THRESOLD,
                  .upperThresholdFraction = DEFAULT_THRESHOLD_FRACTION,
                  .reportTimeBase = DEFAULT_REPORT_TIME_BASE,
                  .minReportTime = DEFAULT_MIN_REPORT_TIME,
                  .maxReportTime = DEFAULT_MAX_REPORT_TIME,
                  .hysteresis = DEFAULT_HYSTERESIS,
               };
               return defaultConfiguration;
            }

            inline void checkAndCorrect()
            {
               if ( reportTimeBase > MAX_REPORT_TIME_BASE )
               {
                  reportTimeBase = MAX_REPORT_TIME_BASE;
               }

               if ( minReportTime > maxReportTime )
               {
                  minReportTime = maxReportTime;
               }

               if ( upperThresholdFraction > MAX_THRESHOLD_FRACTION )
               {
                  upperThresholdFraction = MAX_THRESHOLD_FRACTION;
               }

               if ( lowerThresholdFraction > MAX_THRESHOLD_FRACTION )
               {
                  lowerThresholdFraction = MAX_THRESHOLD_FRACTION;
               }

               if ( lowerThreshold >= upperThreshold )
               {
                  lowerThreshold = upperThreshold;
                  if ( lowerThresholdFraction > upperThresholdFraction )
                  {
                     lowerThresholdFraction = upperThresholdFraction;
                  }
               }
            }
      };

      class EepromConfiguration : public ConfigurationManager::EepromConfigurationTmpl<Configuration>
      {
         public:

            int8_tx lowerThreshold;

            uint8_tx lowerThresholdFraction;

            int8_tx upperThreshold;

            uint8_tx upperThresholdFraction;

            uint8_tx reportTimeBase;

            uint8_tx minReportTime;

            uint8_tx maxReportTime;

            uint8_tx hysteresis;

            uint16_tx reserve;
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
         timeSinceReport( 1 ), currentEvent( 0 ), lastEvent( 0 ), errorCounter( 0 )
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

      inline uint8_t getTimeSinceReport() const
      {
         return timeSinceReport;
      }

      inline void setTimeSinceReport( uint8_t p_timeSinceReport )
      {
         timeSinceReport = p_timeSinceReport;
      }

      ////    Attributes    ////

      static const uint8_t debugLevel;

      Status lastStatus;

      uint8_t timeSinceReport;

      uint8_t currentEvent;

      uint8_t lastEvent;

      uint8_t errorCounter;

      ////    Relations and components    ////

      EepromConfiguration* configuration;

};

#endif
