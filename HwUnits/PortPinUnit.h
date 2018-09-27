/*
 * PortPinUnit.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_PortPinUnit_H
#define HwUnits_PortPinUnit_H

#include "HwUnits.h"
#include <DigitalOutput.h>
#include <ConfigurationManager.h>

class CriticalSection;

class Event;

class HACF;

class PortPin;

class Scheduler;

class evMessage;

class PortPinUnit : public Reactive
{
   public:

      enum State
      {
         OFF,
         ON
      };

      static const uint8_t MAX_BRIGHTNESS = 100;
      static const uint8_t FEEDBACK_BRIGHTNESS = MAX_BRIGHTNESS;

      struct Configuration
      {
         static const uint8_t DEFAULT_DUTY_OFFSET = 0;
         static const uint8_t DEFAULT_MIN_DUTY = 0;
         static const uint8_t DEFAULT_OPTIONS = 0x06; // driveOff, driveOn, !invert
         static const uint8_t MAX_OPTIONS_MASK = 7;
         static const uint16_t DEFAULT_TIME_BASE = 1000;

         struct Options
         {
            uint8_t invert : 1;
            uint8_t driveOn : 1;
            uint8_t driveOff : 1;
            uint8_t reserved : 5;
         };

         union Option
         {
            uint8_t mask;
            Options options;
         };

         ////    Attributes    ////

         uint8_t dutyOffset;

         uint8_t minDuty;

         uint16_t timeBase;

         Option option;

         ////    Operations    ////

         static inline Configuration getDefault()
         {
            Configuration defaultConfiguration =
            {
               .dutyOffset = DEFAULT_DUTY_OFFSET,
               .minDuty = DEFAULT_MIN_DUTY,
               .timeBase = DEFAULT_TIME_BASE,
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

            uint8_tx dutyOffset;

            uint8_tx minDuty;

            uint16_tx timeBase;

            XEeprom<Configuration::Option> option;

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
               SET_CONFIGURATION,
               OFF,
               ON,
               TOGGLE,
               GET_STATUS,
            };

            struct On
            {
               uint16_t duration;
            };

            struct Toggle
            {
               uint8_t offTime;
               uint8_t onTime;
               uint8_t quantity;
            };

            union Parameter
            {
               Configuration setConfiguration;
               On on;
               Toggle toggle;
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

               EVENT_OFF = HACF::EVENTS_START,
               EVENT_ON,
               EVENT_TOGGLE
            };

            union Parameter
            {
               Configuration configuration;
               uint8_t status;
               uint16_t duration;
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

            inline uint8_t isOn()
            {
               if ( getResponse() == EVENT_ON )
               {
                  return getParameter().status;
               }
               return 0;
            }

            Parameter& setConfiguration();

            void setEvent( uint8_t event );

            void setEventOn( uint16_t duration );

            void setStatus( uint8_t status );

            ////    Attributes    ////

         private:

            Parameter params;
      };

      ////    Constructors and destructors    ////

      PortPinUnit( PortPin _hardware );

      ////    Operations    ////

      void cmdToggle( const Command::Toggle& parameter );

      void enableFeedback( bool enable = true );

      bool handleRequest( HACF* message );

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size )
      {
         return allocOnce( size );
      }

      void updateConfiguration();

   protected:

      bool run();

      void updateQuantity();

   private:

      void updateHw();

      ////    Additional operations    ////

   public:

      inline bool getFeedback() const
      {
         return feedback;
      }

      inline void setConfiguration( EepromConfiguration* _config )
      {
         configuration = _config;
      }

      DigitalOutput* getHardware() const;

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      inline uint8_t getQuantity() const
      {
         return quantity;
      }

      inline void setQuantity( uint8_t p_quantity )
      {
         quantity = p_quantity;
      }

      inline uint8_t getOnTime() const
      {
         return onTime;
      }

      inline void setOnTime( uint8_t p_onTime )
      {
         onTime = p_onTime;
      }

      inline uint8_t getOffTime() const
      {
         return offTime;
      }

      inline void setOffTime( uint8_t p_offTime )
      {
         offTime = p_offTime;
      }

      inline uint16_t getDuration() const
      {
         return duration;
      }

      inline void setDuration( uint16_t p_duration )
      {
         duration = p_duration;
      }

      inline void setFeedback( bool p_feedback )
      {
         feedback = p_feedback;
      }

      inline bool getDriveOnState() const
      {
         return driveOnState;
      }

      inline void setDriveOnState( bool p_driveOnState )
      {
         driveOnState = p_driveOnState;
      }

      inline bool getDriveOffState() const
      {
         return driveOffState;
      }

      inline void setDriveOffState( bool p_driveOffState )
      {
         driveOffState = p_driveOffState;
      }

      ////    Attributes    ////

      static const uint8_t debugLevel;

      uint8_t quantity;

      uint8_t onTime;

      uint8_t offTime;

      uint16_t duration;

      bool feedback : 1;

      bool driveOnState : 1;

      bool driveOffState : 1;

      ////    Relations and components    ////

      EepromConfiguration* configuration;

      DigitalOutput hardware;

};

#endif
