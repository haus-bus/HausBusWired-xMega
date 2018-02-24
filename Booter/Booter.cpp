/*
 * Booter.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Booter.h"

const ModuleId moduleId =
{
   "$MOD$ "MOD_ID,
   BOOTER_SIZE,
   Release::MAJOR,
   Release::MINOR,
   CONTROLLER_ID,
   0
};

const uint8_t Booter::debugLevel( DEBUG_LEVEL_OFF );

HACF::ControlFrame* Booter::message( 0 );

bool Booter::isFirmwareValid( false );

bool Booter::startFirmware( false );

bool Booter::downloadAllowed( false );

BooterHw Booter::hardware;

ModuleId Booter::installedMod;

NO_RETURN void Booter::start()
{
   hardware.configure();
   HACF::deviceId = HwConfiguration::HomeAutomation::instance().getDeviceId();

   checkFirmware();
   while ( true )
   {
      message = hardware.getMessage();
      if ( message )
      {
         if ( handleMessage() && message->getDataLength() )
         {
            message->receiverId = message->senderId;
            message->senderId.setDeviceId( HACF::deviceId );
            message->senderId.setObjectId( HACF::BOOTLOADER_ID );
            hardware.sendMessage();
         }
      }
      if ( startFirmware )
      {
         startApplication();
      }
   }
}

int main( int argc, char* argv[] )
{
   int status = 0;
   {
      Booter::start();
   }
   return status;
}


SystemTime::time_t SystemTime::now()
{
   return 0;
}