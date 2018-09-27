/*
 * LoggerMemory.h
 *
 *  Created on: 11.04.2016
 *      Author: viktor.pankraz
 */

#ifndef LOGGERMEMORY_H_
#define LOGGERMEMORY_H_

#include <HwUnits.h>
#include <FileSystems/FatSystem.h>
#include <Reactive.h>

class LoggerMemory : public Reactive
{
   class ErrorCode
   {
      public:
         enum ErrorCodes
         {
            NO_ERROR,
            MEMORY_NOT_AVAILABLE
         };
   };

   class Command
   {
      public:

         enum Commands
         {
            ERASE = HACF::COMMANDS_START,
            GET_LOGS,
         };

         union Parameter
         {
            uint32_t sector;
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
            LOG_DATA = HACF::RESULTS_START,
         };

         union Parameter
         {
            uint8_t logData[512];
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

         Parameter& setLogData();

         ////    Attributes    ////

      private:

         Parameter params;
   };

   public:

      enum SubStates
      {
         OPEN_FILE,
         LOGGING,
         DRIVE_NOT_READY,

      };

      ////    Constructors and destructors    ////

      LoggerMemory( IDiskIo* _disk );

      ////    Operations    ////

      virtual bool notifyEvent( const Event& event );

      void run();

   private:

      void sendError( ErrorCode::ErrorCodes code );

      bool updateLogfileName();

      FatSystem::Result prepareLogFile();

      ////    Attributes    ////

   private:

      static const uint8_t debugLevel;

      static char logFileName[];

      FatSystem::File myLogFile;

      FatSystem::FileSystem* myFileSystem;

}
;

#endif /* LOGGERMEMORY_H_ */
