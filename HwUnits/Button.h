/*
 * Button.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_Button_H
#define HwUnits_Button_H

#include <ConfigurationManager.h>

#include "HwUnits.h"

class CriticalSection;

class Event;

class HACF;

class Led;

class Scheduler;

class evMessage;

class Button : public Reactive
{
   public:

      enum State
      {
         RELEASED,
         PRESSED,
         CLICKED,
         CLICKED_PRESSED,
         HOLD
      };

      struct Configuration
      {
         static const uint8_t DEFAULT_HOLD_TIMEOUT = 100;
         static const uint8_t DEFAULT_DOUBLE_CLICK_TIMEOUT = 50;
         static const uint8_t DEFAULT_EVENTS = 0xA1;

         struct EventBits
         {
            uint8_t notifyOnCovered : 1;
            uint8_t notifyOnClicked : 1;
            uint8_t notifyOnDoubleClicked : 1;
            uint8_t notifyOnHoldStart : 1;
            uint8_t notifyOnHoldEnd : 1;
            uint8_t notifyOnFree : 1;
            uint8_t reserved : 1;
            uint8_t feedbackEnabled : 1;
         };

         union Event
         {
            uint8_t mask;
            EventBits bit;
         };

         ////    Attributes    ////

         uint8_t holdTimeout;

         uint8_t doubleClickTimeout;

         Event events;

         ////    Functions    ////

         static inline Configuration getDefault()
         {
            Configuration defaultConfiguration =
            {
               .holdTimeout = DEFAULT_HOLD_TIMEOUT,
               .doubleClickTimeout = DEFAULT_DOUBLE_CLICK_TIMEOUT,
               .events = { DEFAULT_EVENTS }
            };
            return defaultConfiguration;
         }

         inline void checkAndCorrect()
         {
            if ( events.mask == 0xFF )
            {
               events.mask = DEFAULT_EVENTS;
            }
         }

      };

      class EepromConfiguration : public ConfigurationManager::EepromConfigurationTmpl<Configuration>
      {
         public:

            uint8_tx holdTimeout;

            uint8_tx doubleClickTimeout;

            XEeprom<Configuration::Event> events;

            uint16_tx reserve;

            Configuration::Event getEvents() const
            {
               return events.operator*();
            }
      };

      class Command
      {
         public:

            enum Commands
            {
               GET_CONFIGURATION = HACF::COMMANDS_START,
               SET_CONFIGURATION,
               ENABLE_EVENTS,
               GET_STATUS
            };

            struct EnableEvents
            {
               uint8_t enable;
               uint8_t disabledDuration;
            };

            union Parameter
            {
               Configuration setConfiguration;
               EnableEvents enableEvents;
            };

            enum EEnable
            {
               False = 0,
               True,
               Invert
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

               EVENT_COVERED = HACF::EVENTS_START,
               EVENT_CLICKED,
               EVENT_DOUBLE_CLICKED,
               EVENT_HOLD_START,
               EVENT_HOLD_END,
               EVENT_FREE,
            };

            union Parameter
            {
               Configuration configuration;
               uint8_t buttonEvent;
               uint8_t state;
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

            inline bool isCovered();

            Parameter& setConfiguration();

            inline void setEvent( uint8_t event );

            inline void setState( uint8_t _state );

            ////    Attributes    ////

         private:

            Parameter params;
      };

      ////    Constructors and destructors    ////

      Button( uint8_t _id );

      ////    Operations    ////

      bool handleRequest( HACF* message );

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size );

      void tick();

      void updateState( uint8_t pressed );

   private:

      void notifyNewState( Configuration::Event event ) const;

      ////    Additional operations    ////

   public:

      inline void setConfiguration( EepromConfiguration* _conf )
      {
         configuration = _conf;
      }

      Led* getFeedbackLed() const;

      void setFeedbackLed( Led* p_Led );

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      inline uint8_t getDisabledDuration() const
      {
         return disabledDuration;
      }

      inline void setDisabledDuration( uint8_t p_disabledDuration )
      {
         disabledDuration = p_disabledDuration;
      }

   private:

      inline Configuration::Event getEnabledEvents() const
      {
         return enabledEvents;
      }

      inline void setEnabledEvents( Configuration::Event p_enabledEvents )
      {
         enabledEvents = p_enabledEvents;
      }

      ////    Attributes    ////

   public:


   protected:

      static const uint8_t debugLevel;

      uint8_t disabledDuration;

   private:

      Configuration::Event enabledEvents;

      ////    Relations and components    ////

   protected:

      EepromConfiguration* configuration;

      Led* feedbackLed;

};

inline void* Button::operator new( size_t size )
{
   return allocOnce( size );
}

inline bool Button::Response::isCovered()
{
   return getResponse() == EVENT_COVERED;
}

inline void Button::Response::setEvent( uint8_t event )
{
   controlFrame.setDataLength( sizeof( getResponse() ) );
   setResponse( event );
}

inline void Button::Response::setState( uint8_t _state )
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().state ) );
   setResponse( STATUS );
   if ( ( _state == RELEASED ) || ( _state == CLICKED ) )
   {
      getParameter().state = RELEASED;
   }
   else
   {
      getParameter().state = PRESSED;
   }
}

#endif
