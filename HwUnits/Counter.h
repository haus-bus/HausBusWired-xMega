/*
 * Counter.h
 *
 *  Created on: 22.10.2015
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_Counter_H
#define HwUnits_Counter_H

#include "HwUnits.h"
#include <ConfigurationManager.h>

class Counter : public Reactive
{
   public:

      struct Configuration
      {
         enum EMode
         {
            DECREMENT_FALLING = 0x02,
            INCREMENT_FALLING = 0x03,
            DEREMENT_RISING = 0x04,
            INCREMENT_RISING = 0x05,
            DEREMENT_EITHER = 0x06,
            INCREMENT_EITHER = 0x07,
         };

         static const uint8_t DEFAULT_MODE = INCREMENT_FALLING;
         static const uint16_t DEFAULT_REPORT_TIME = 60;
         static const uint32_t DEFAULT_THRESHOLD = 0;
         static const uint32_t DEFAULT_VALUE = 0;

         struct Mode
         {
            uint8_t increment : 1;
            uint8_t falling : 1;
            uint8_t rising : 1;
         };

         union UMode
         {
            uint8_t mask;
            Mode bit;
         };

         ////    Attributes    ////

         UMode mode;

         uint16_t reportTime;

         uint32_t threshold;

         ////    Operations    ////

         static inline Configuration getDefault()
         {
            Configuration defaultConfiguration =
            {
               .mode = { DEFAULT_MODE },
               .reportTime = DEFAULT_REPORT_TIME,
               .threshold = DEFAULT_THRESHOLD
            };
            return defaultConfiguration;
         }

         inline void checkAndCorrect()
         {
            if ( mode.mask > INCREMENT_EITHER )
            {
               mode.mask = DEFAULT_MODE;
            }
         }
      };

      class EepromConfiguration : public ConfigurationManager::EepromConfigurationTmpl<Configuration>
      {
         public:

            XEeprom<Configuration::UMode> mode;

            uint16_tx reportTime;

            uint32_tx threshold;

            uint8_tx reserve1;

            uint16_tx reserve2;

            uint32_tx value;

            inline Configuration::Mode getMode() const
            {
               return mode.operator*().bit;
            }
      };

      class Command
      {
         public:

            enum Commands
            {
               GET_CONFIGURATION = HACF::COMMANDS_START,
               SET_CONFIGURATION,
               GET_VALUE,
               SET_VALUE
            };

            union Parameter
            {
               Configuration setConfiguration;
               uint32_t value;
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
               VALUE,
               STATUS,
            };

            union Parameter
            {
               Configuration configuration;
               uint32_t value;
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

            inline void setValue( uint32_t value );

            inline void setStatus( uint32_t value );

            ////    Attributes    ////

         private:

            Parameter params;
      };

      ////    Constructors and destructors    ////

      Counter( uint8_t _id );

      ////    Operations    ////

      bool handleRequest( HACF* message );

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size );

      void updateState( uint8_t newState );

   private:

      void tick();

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

   private:

      ////    Attributes    ////

   public:

   protected:

      static const uint8_t debugLevel;

   private:

      Configuration::Mode mode;

      uint16_t timeToReport;

      uint32_t value;

      uint32_t lastValue;



      ////    Relations and components    ////

   protected:

      EepromConfiguration* configuration;

};

inline void* Counter::operator new( size_t size )
{
   return allocOnce( size );
}

inline void Counter::Response::setValue( uint32_t value )
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().value ) );
   setResponse( VALUE );
   getParameter().value = value;
}

inline void Counter::Response::setStatus( uint32_t value )
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().value ) );
   setResponse( STATUS );
   getParameter().value = value;
}

#endif
