/*
 * DS1307.h
 *
 *  Created on: 14.03.2016
 *      Author: viktor.pankraz
 */

#include <HwUnits.h>
#include <Peripherals/Twi.h>

#ifndef DS1307_H_
#define DS1307_H_

class DS1307 : public Reactive
{
   static const uint8_t ADDRESS = 0x68;

   public:

      class ErrorCode
      {
         public:
            enum ErrorCodes
            {
               NO_ERROR,
               TWI_BUSY,
               TWI_READ_FAILED,
               TWI_WRITE_FAILED,
               CLOCK_HALTED
            };
      };

      class TimeElements
      {
         public:
            uint8_t dayOfWeek;
            uint8_t date;
            uint8_t month;
            uint16_t year;
            uint8_t hours;
            uint8_t minutes;
            uint8_t seconds;

            inline void reset()
            {
               dayOfWeek = 1;
               date = 1;
               month = 1;
               year = 2000;
               hours = 0;
               minutes = 0;
               seconds = 0;
            }
      };

      class Command
      {
         public:

            enum Commands
            {
               GET_TIME = HACF::COMMANDS_START,
               SET_TIME,
            };

            union Parameter
            {
               TimeElements te;
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
               TIME = HACF::RESULTS_START,
            };

            union Parameter
            {
               TimeElements te;
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

            Parameter& setTime();

            ////    Attributes    ////

         private:

            Parameter params;
      };

      ////    Constructors and destructors    ////

      DS1307( Twi& _twi );

      ////    Operations    ////

      virtual bool notifyEvent( const Event& event );

      uint8_t setTime( TimeElements& timeElements );

      uint8_t getTime( TimeElements& timeElements );

      uint8_t setControlRegister( uint8_t data );

      bool isRunning();

      void run();

      ////    Attributes    ////

   private:

      static const uint8_t debugLevel;

      Twi* twi;
};

#endif /* DS1307_H_ */
