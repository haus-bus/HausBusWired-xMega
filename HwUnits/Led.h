/*
 * Led.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_Led_H
#define HwUnits_Led_H

#include "HwUnits.h"
#include "PortPinUnit.h"

class CriticalSection;

class Event;

class HACF;

class PortPin;

class PwmOutput;

class Scheduler;

class evMessage;

class Led : public PortPinUnit
{
   public:

      class Command;

      class Response;

      static const uint8_t MAX_BRIGHTNESS = 100;
      static const uint8_t FEEDBACK_BRIGHTNESS = MAX_BRIGHTNESS;

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
               SET_MIN_BRIGHTNESS
            };

            struct On
            {
               uint8_t brightness;
               uint16_t duration;
            };

            struct Blink
            {
               uint8_t brightness;
               PortPinUnit::Command::Toggle toggleParams;
            };

            struct SetMinBrightness
            {
               uint8_t brightness;
               uint16_t duration;
            };

            // ## type Parameter
            union Parameter
            {
               PortPinUnit::Configuration setConfiguration;
               On on;
               Blink blink;
               SetMinBrightness setMinBrightness;
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
               EVENT_BLINK
            };

            union Parameter
            {
               PortPinUnit::Configuration configuration;
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

            inline uint8_t isOn();

            void setBrightness( uint8_t brightness );

            Parameter& setConfiguration();

            void setEvent( uint8_t event );

            void setStatus( uint8_t status );

            ////    Attributes    ////

         private:

            Parameter params;

      };

      ////    Constructors and destructors    ////

      Led( PortPin _hardware );

      ////    Operations    ////

      void cmdBlink( const Command::Blink& parameter );

      void cmdOn( const Command::On& parameter );

      void enableFeedback( bool enable = true );

      uint8_t getHwBrightness() const;

      bool handleRequest( HACF* message );

      virtual bool notifyEvent( const Event& event );

   private:

      void setBrightness( uint8_t p_brightness );

      void updateHw();

      ////    Additional operations    ////

   public:

      inline uint8_t getBrightness() const
      {
         return brightness;
      }

      inline uint8_t getDimmOffset() const
      {
         return dimmOffset;
      }

      inline uint8_t getMinBrightness() const
      {
         return minBrightness;
      }

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

   private:

      inline void setDimmOffset( uint8_t p_dimmOffset )
      {
         dimmOffset = p_dimmOffset;
      }

      inline void setMinBrightness( uint8_t p_minBrightness )
      {
         minBrightness = p_minBrightness;
      }

      ////    Attributes    ////

   protected:

      static const uint8_t debugLevel;

   private:

      uint8_t brightness;

      uint8_t dimmOffset;

      uint8_t minBrightness;

      uint8_t brightnessBeforeBlink;

      uint8_t stateBeforeBlink;

};

inline uint8_t Led::Response::isOn()
{
   if ( getResponse() == EVENT_ON )
   {
      return getParameter().status;
   }
   return 0;
}

#endif
