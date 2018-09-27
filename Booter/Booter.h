/*
 * BooterHw.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Booter_H
#define Booter_H

#include <Protocols/HACF.h>
#include <HomeAutomationHw.h>
#include <HomeAutomationInterface.h>
#include <Security/ModuleId.h>
#include <Security/Crc8.h>
#include <SystemBoards.h>
#include <DefaultTypes.h>
#include <Release.h>
#include "BooterHw.h"

class BooterHw;

extern MOD_ID_SECTION const ModuleId moduleId;

class Booter
{
   ////    Operations    ////

   public:

      inline static uint16_t getId()
      {
         return HACF::BOOTLOADER_ID;
      }

      static void start();

   private:

      inline static void checkFirmware();

      inline static void cmdGetConfiguration();

      inline static void cmdGetModuleId( uint8_t index );

      inline static const void cmdPing();

      inline static void cmdReadMemory(
         HomeAutomationInterface::Command::ReadMemory& parameter );

      inline static const void cmdReset();

      inline static void cmdSetConfiguration(
         HomeAutomationConfiguration& configuration )
      {
         DEBUG_H2( getId(), FSTR( ".setConfiguration()" ) );
         message->setDataLength( 0 );
         HomeAutomationConfiguration::instance().set( configuration );
      }

      inline static void cmdWriteMemory(
         HomeAutomationInterface::Command::WriteMemory& parameter )
      {
         DEBUG_H2( getId(), FSTR( ".writeMemory()" ) );
#ifdef _DEBUG_
         WARN_1( FSTR( "writeMemory not possible in debug mode" ) );
         getResponse()->setMemoryStatus( Stream::LOCKED );
#else
         uint16_t dataLength = message->getDataLength() - sizeof( parameter.address )
                               - 1; // sizeof( command )
         if ( !downloadAllowed )
         {
            static uint8_t buffer[3 * APP_SECTION_PAGE_SIZE];
            if ( ( parameter.address + dataLength ) <= sizeof( buffer ) )
            {
               memcpy( &buffer[parameter.address], parameter.data, dataLength );
               if ( ( parameter.address + dataLength )
                    < ( _VECTORS_SIZE + sizeof( ModuleId ) ) )
               {
                  // get more data into buffer to analyze ModuleId
                  getResponse()->setMemoryStatus( Stream::SUCCESS );
                  return;
               }
               ModuleId* modId = (ModuleId*) &buffer[_VECTORS_SIZE];
               if ( modId->getChecksum() == Crc8::hasError( modId, sizeof( ModuleId ) - 1 ) )
               {
                  if ( modId->firmwareId == CONTROLLER_ID )
                  {
                     if ( modId->majorRelease == Release::MAJOR )
                     {
                        if ( modId->minorRelease >= Release::MINOR )
                        {
                           if ( Flash::write( 0, buffer, Flash::getPageSize() ) == Flash::getPageSize() )
                           {
                              downloadAllowed = true;
                              if ( parameter.address >= ( 2 * APP_SECTION_PAGE_SIZE ) )
                              {
                                 downloadAllowed = ( Flash::write( APP_SECTION_PAGE_SIZE, &buffer[APP_SECTION_PAGE_SIZE], Flash::getPageSize() ) == Flash::getPageSize() );
                              }

                           }
                        }
                     }
                  }
               }
            }
         }
         Stream::Status result = Stream::ABORTED;
         if ( downloadAllowed )
         {
            if ( Flash::write( parameter.address, parameter.data, dataLength ) == dataLength )
            {
               result = Stream::SUCCESS;
            }
         }
         else
         {
            // start FW if download of this data is not allowed
            startFirmware = isFirmwareValid;
         }
         getResponse()->setMemoryStatus( result );
#endif
      }

      inline static HomeAutomationInterface::Response* getResponse()
      {
         return reinterpret_cast<HomeAutomationInterface::Response*>( ( (uint16_t) message )
                                                                      - sizeof( HACF::Header ) );
      }

      inline static bool handleMessage();

      inline static void startApplication();

      ////    Additional operations    ////

   public:

      inline static HACF::ControlFrame* getMessage()
      {
         return message;
      }

      inline static void setMessage( HACF::ControlFrame* p_message )
      {
         message = p_message;
      }

      inline static bool getIsFirmwareValid()
      {
         return isFirmwareValid;
      }

      inline static void setIsFirmwareValid( bool p_isFirmwareValid )
      {
         isFirmwareValid = p_isFirmwareValid;
      }

      inline static bool getStartFirmware()
      {
         return startFirmware;
      }

      inline static void setStartFirmware( bool p_startFirmware )
      {
         startFirmware = p_startFirmware;
      }

      inline static BooterHw* getHardware()
      {
         return (BooterHw*) &hardware;
      }

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      inline static void writeMemoryEnd()
      {
         PORTR.OUTSET = Pin0;
      }

      inline static void writeMemoryStart()
      {
         PORTR.OUTCLR = Pin0;
      }

      ////    Attributes    ////

      static const uint8_t debugLevel;

   public:

      static HACF::ControlFrame* message;

      static bool isFirmwareValid;

      static bool startFirmware;

      static bool downloadAllowed;

      static BooterHw hardware;

      static ModuleId installedMod;
};

inline void Booter::checkFirmware()
{
   DEBUG_H2( getId(), FSTR( ".checkFirmware()" ) );

   if ( HomeAutomationHw::getModuleId( 0, &installedMod ) )
   {
      uint32_t fCrc;
      uint32_t cCrc = Flash::getRangeCRC( 0, installedMod.getSize() - 1 );
      Flash::read( installedMod.getSize(), &fCrc, sizeof( fCrc ) );

      DEBUG_M2( FSTR( "name:    " ), installedMod.name ); DEBUG_M2( FSTR( "size:    0x" ), installedMod.size ); DEBUG_M4( FSTR( "release: " ), installedMod.majorRelease, '.',
                                                                                                                          installedMod.minorRelease ); DEBUG_M2( FSTR( "cCRC:    0x" ), cCrc ); DEBUG_M2( FSTR( "fCRC:    0x" ), fCrc );

      if ( ( fCrc == cCrc ) && ( Release::MAJOR == installedMod.majorRelease ) )
      {
         isFirmwareValid = true;
         startFirmware = true;
         return;
      }

   }
   DEBUG_M1( FSTR( "invalid" ) );
}

inline void Booter::cmdGetConfiguration()
{
   DEBUG_H2( getId(), FSTR( ".getConfiguration()" ) );
   HomeAutomationConfiguration::instance().get(
      getResponse()->setConfiguration( getFckE() ) );
}

inline void Booter::cmdGetModuleId( uint8_t index )
{
   DEBUG_H2( getId(), FSTR( ".getModuleId()" ) );
   HomeAutomationInterface::Response* response = getResponse();
   if ( !HomeAutomationHw::getModuleId( --index, response->setModuleId() ) )
   {
      response->setErrorCode( HomeAutomationInterface::ErrorCode::MODULE_NOT_EXISTS );
   }
   else if ( !isFirmwareValid && !index )
   {
      response->setErrorCode( HomeAutomationInterface::ErrorCode::INVALID_FW_LOADED );
   }
}

inline const void Booter::cmdPing()
{
   DEBUG_H2( getId(), FSTR( ".ping()" ) );
   getResponse()->setPong();
   startFirmware = false;
}

inline void Booter::cmdReadMemory(
   HomeAutomationInterface::Command::ReadMemory& parameter )
{
   DEBUG_H2( getId(), FSTR( ".readMemory()" ) );

   HomeAutomationInterface::Response* response = getResponse();
   uint8_t* dest = response->setReadMemory( parameter.address,
                                            parameter.length );
   if ( parameter.address & HomeAutomationInterface::DATA_SECTION_MASK )
   {
      uint8_t* source = reinterpret_cast<uint8_t*>( parameter.address & 0xFFFF );
      uint16_t length = parameter.length;
      while ( length-- )
      {
         dest[length] = source[length];
      }
   }
   else
   {
      if ( Flash::read( parameter.address, dest, parameter.length ) != parameter.length )
      {
         response->setMemoryStatus( Stream::ABORTED );
      }
   }
}

inline const void Booter::cmdReset()
{
   DEBUG_H2( getId(), FSTR( ".reset()" ) );
   ResetSystem::reset();
}

inline bool Booter::handleMessage()
{
   bool consumed = true;

   HomeAutomationInterface::Command* data
      = reinterpret_cast<HomeAutomationInterface::Command*>( message->getData() );
   uint8_t command = data->getCommand();
   if ( command < HACF::RESULTS_START )
   {
      if ( command == HomeAutomationInterface::Command::RESET )
      {
         cmdReset();
      }
// else if( command == HomeAutomationInterface::Command::GENERATE_RANDOM_DEVICE_ID )
// {
// HACF::deviceId = ( BooterHw::getNewDeviceId() & 0x7FFF );
// HomeAutomationHw::Configuration::instance().setDeviceId( HACF::deviceId );
// }
      else if ( command == HomeAutomationInterface::Command::GET_MODULE_ID )
      {
         cmdGetModuleId( data->parameter.getModuleId.index );
      }
      else if ( command == HomeAutomationInterface::Command::GET_CONFIGURATION )
      {
         cmdGetConfiguration();
      }
      else if ( command == HomeAutomationInterface::Command::SET_CONFIGURATION )
      {
         cmdSetConfiguration( data->parameter.setConfiguration );
      }
      else if ( command == HomeAutomationInterface::Command::READ_MEMORY )
      {
         cmdReadMemory( data->parameter.readMemory );
      }
      else if ( command == HomeAutomationInterface::Command::WRITE_MEMORY )
      {
         writeMemoryStart();
         cmdWriteMemory( data->parameter.writeMemory );
         if ( message->isBroadcast() )
         {
            // do not answer for broadcast downloads to prevent data collision
            message->setDataLength( 0 );
         }
         writeMemoryEnd();
      }
      else if ( command == HomeAutomationInterface::Command::PING )
      {
         cmdPing();
      }
      else
      {
         consumed = false;
      }
   }
   else
   {
      consumed = false;
   }

   return consumed;
}

inline void Booter::startApplication()
{
#ifdef EIND
   EIND = 0;
#endif

   void ( *start )( void ) = NULL;
   start();
}

#endif
