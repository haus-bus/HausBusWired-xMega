/*
 * HmwDevice.cpp
 *
 * Created: 06.10.2017 22:40:44
 * Author: viktor.pankraz
 */


#include "HmwDevice.h"
#include "HmwChannel.h"
#include "HmwMsgGetFwVersion.h"
#include "HmwMsgGetHwVersion.h"
#include "HmwMsgReadEeprom.h"
#include "HmwMsgWriteFlash.h"
#include "HmwMsgEepromMap.h"
#include "HmwMsgGetLevel.h"
#include "HmwMsgGetSerial.h"
#include "HmwMsgWriteEeprom.h"
#include "HmwMsgSetLevel.h"

#include <Peripherals/WatchDog.h>
#include <Peripherals/ResetSystem.h>


uint8_t HmwDevice::deviceType( 0 );

uint32_t HmwDevice::ownAddress( DEFAULT_ADDRESS );

HmwDevice::BasicConfig* HmwDevice::basicConfig( 0 );

HmwDevice::PendingActions HmwDevice::pendingActions;

HmwLinkReceiver* HmwDevice::linkReceiver( NULL );

HmwLinkSender* HmwDevice::linkSender( NULL );

const uint8_t HmwDevice::debugLevel( DEBUG_LEVEL_LOW );


// The loop function is called in an endless loop
void HmwDevice::loop()
{

   for ( uint8_t i = 0; i < HmwChannel::getNumChannels(); i++ )
   {
      HmwChannel::getChannel( i )->loop( i );
      handlePendingInMessages();
      HmwStream::handlePendingOutMessages();
   }
   handlePendingActions();
   // handleConfigButton();
   WatchDog::reset();
}

void HmwDevice::handlePendingInMessages()
{
   HmwMessageBase* request = HmwStream::getMessage();
   if ( request )
   {
      if ( request->isOnlyForMe() && ( request->isACK() || request->isInfoLevel() ) )
      {
         HmwStream::notifyReceivedAckOrInfoLevel( *request );
      }
      if ( !request->isACK() )
      {
         HmwMessageBase answer( *request );
         if ( processMessage( answer ) )
         {
            HmwStream::sendMessage( answer );
         }
      }

      delete request;
   }
}

void HmwDevice::handlePendingActions()
{
   if ( pendingActions.readConfig )
   {
      checkConfig();
      pendingActions.readConfig = false;
   }
   if ( pendingActions.startBooter )
   {
      ResetSystem::reset();
   }
   if ( pendingActions.resetSystem )
   {
      // wait for WatchDog to reset
      while ( 1 )
      {
      }
   }
   handleAnnouncement();
}

void HmwDevice::handleAnnouncement()
{
   if ( isAnnouncementPending() && HmwStream::isIdle() )
   {
      if ( HmwDevice::announce() == Stream::SUCCESS )
      {
         clearPendingAnnouncement();
      }
   }
}

void HmwDevice::checkConfig()
{
   uint32_t address = changeEndianness( basicConfig->ownAddress );
   if ( ownAddress != address )
   {
      // address was changed, update and send new announce message
      setOwnAddress( address );
      pendingActions.announce = true;
   }
   if ( ( basicConfig->loggingTime < 50 ) || ( basicConfig->loggingTime > 250 ) )
   {
      basicConfig->loggingTime = 50;
   }

   // check all channels
   for ( uint8_t i = 0; i < HmwChannel::getNumChannels(); i++ )
   {
      HmwChannel::getChannel( i )->checkConfig();
   }
}

bool HmwDevice::processMessage( HmwMessageBase& msg )
{
   if ( !msg.isForMe() || !msg.isInfo() )
   {
      return false;
   }

   bool isValid = true;
   bool ackOnly = true;

   if ( msg.isCommand( HmwMessageBase::READ_CONFIG ) )
   {
      DEBUG_M1( FSTR( "C: read config" ) );
      pendingActions.readConfig = true;
   }
   else if ( msg.isCommand( HmwMessageBase::WRITE_FLASH ) )
   {
      DEBUG_M1( FSTR( "C: Write Flash" ) );
      if ( msg.getFrameDataLength() > 6 )
      {
         HmwMsgWriteFlash* msgWriteFlash = (HmwMsgWriteFlash*)&msg;
         static uint8_t buffer[ APP_SECTION_PAGE_SIZE ];
         Flash::address_t address = msgWriteFlash->getAddress();
         uint8_t length = msgWriteFlash->getLength();
         memcpy( &buffer[address & ( Flash::getPageSize() - 1 )], msgWriteFlash->getData(), length );

         if ( ( address + length ) % Flash::getPageSize() )
         {
            // read more bytes
         }
         else if ( Flash::write( address & ~( Flash::getPageSize() - 1 ), buffer, Flash::getPageSize() ) != Flash::getPageSize() )
         {
            DEBUG_M2( FSTR( "E: Flash::write failed:" ), msg.getFrameDataLength() );
            isValid = false;
         }
      }
      else
      {
         DEBUG_M2( FSTR( "E: wrong data length :" ), msg.getFrameDataLength() );
         isValid = false;
      }
   }

#ifndef _BOOTER_
   else if ( msg.isCommand( HmwMessageBase::GET_FW_VERSION ) )
   {
      DEBUG_M1( FSTR( "C: get FW version" ) );
      ( (HmwMsgGetFwVersion*)&msg )->setupResponse( ( Release::MAJOR << 8 ) | Release::MINOR );
      ackOnly = false;
   }
   else if ( msg.isCommand( HmwMessageBase::GET_HARDWARE_VERSION ) )
   {
      DEBUG_M1( FSTR( "C: HWVer,Typ" ) );
      ( (HmwMsgGetHwVersion*)&msg )->setupResponse( HmwDevice::deviceType, basicConfig->hwVersion );
      ackOnly = false;
   }
   else if ( msg.isCommand( HmwMessageBase::READ_EEPROM ) )
   {
      DEBUG_M1( FSTR( "C: Read EEPROM" ) );
      if ( msg.getFrameDataLength() == 4 )        // Length of incoming data must be 4
      {
         ( (HmwMsgReadEeprom*)&msg )->setupResponse();
         ackOnly = false;
      }
      else
      {
         DEBUG_M2( FSTR( "E: wrong data length :" ), msg.getFrameDataLength() );
         isValid = false;
      }
   }
   else if ( msg.isCommand( HmwMessageBase::GET_EEPROM_MAP ) )
   {
      DEBUG_M1( FSTR( "C: GET_EEPROM_MAP" ) );
      ( (HmwMsgEepromMap*)&msg )->setupResponse();
      ackOnly = false;
   }
   else if ( msg.isCommand( HmwMessageBase::KEY_EVENT ) || msg.isCommand( HmwMessageBase::KEY_SIM ) )
   {
      DEBUG_M1( FSTR( "C: KEY_EVENT" ) );
      if ( linkReceiver )
      {
         HmwMsgKeyEvent* event = ( HmwMsgKeyEvent* )&msg;
         linkReceiver->receiveKeyEvent( event->getSenderAddress(), event->getSourceChannel(), event->getDestinationChannel(), event->isLongPress() );
      }
   }
   else if ( msg.isCommand( HmwMessageBase::GET_LEVEL ) )
   {
      DEBUG_M1( FSTR( "C: GET_LEVEL" ) );
      HmwMsgGetLevel* msgGetLevel = ( HmwMsgGetLevel* )&msg;
      msgGetLevel->setupResponse( get( msgGetLevel->getChannel(), msgGetLevel->getData() ) );
      ackOnly = false;
   }
   else if ( msg.isCommand( HmwMessageBase::WRITE_EEPROM ) )
   {
      DEBUG_M1( FSTR( "C: WRITE_EEPROM" ) );
      HmwMsgWriteEeprom* msgWriteEeprom = ( HmwMsgWriteEeprom* )&msg;
      if ( msg.getFrameDataLength() == ( msgWriteEeprom->getLength() + 4 ) )
      {
         uint16_t offset = msgWriteEeprom->getOffset();
         uint8_t length = msgWriteEeprom->getLength();
         uint8_t* data = msgWriteEeprom->getData();

         // at offset 0 the HW_REV is stored to share between BOOTER and FW
         // it is not allowed to change it with external WRITE_EEPROM command
         if ( offset == 0 )
         {
            offset++;
            data++;
            length--;
         }
         Eeprom::write( offset, data, length );
      }
      else
      {
         DEBUG_M2( FSTR( "E: wrong data length :" ), msg.getFrameDataLength() );
         isValid = false;
      }
   }
   else if ( msg.isCommand( HmwMessageBase::GET_SERIAL ) )
   {
      DEBUG_M1( FSTR( "C: GET_SERIAL" ) );
      ( (HmwMsgGetSerial*)&msg )->setupResponse( ownAddress );
      ackOnly = false;
   }
   else if ( msg.isCommand( HmwMessageBase::START_BOOTER ) )
   {
      DEBUG_M1( FSTR( "C: START_BOOTER" ) );
      pendingActions.startBooter = true;
   }
   else if ( msg.isCommand( HmwMessageBase::RESET ) )
   {
      DEBUG_M1( FSTR( "C: RESET" ) );
      pendingActions.resetSystem = true;
   }
   else if ( msg.isCommand( HmwMessageBase::INFO_LEVEL ) )
   {
      DEBUG_M1( FSTR( "C: INFO_LEVEL" ) );
   }
   else if ( msg.isCommand( HmwMessageBase::SET_ACTOR ) || msg.isCommand( HmwMessageBase::SET_LEVEL ) )
   {
      DEBUG_M1( FSTR( "C: SET_LEVEL" ) );
      HmwMsgSetLevel* msgSetLevel = (HmwMsgSetLevel*)&msg;
      set( msgSetLevel->getChannel(), msgSetLevel->getLength(), msgSetLevel->getData() );

      // return immediately the current data for this channel as feedback
      HmwMsgGetLevel* msgGetLevel = ( HmwMsgGetLevel* )&msg;
      uint8_t length = get( msgGetLevel->getChannel(), msgGetLevel->getData() );
      msgGetLevel->setupResponse( length );
      ackOnly = false;
   }
#endif

   else
   {
      return false;
   }

   if ( isValid && !msg.isBroadcast() )
   {
      msg.convertToResponse( ownAddress, ackOnly );
      return true;
   }
   return false;
}

uint8_t HmwDevice::get( uint8_t channel, uint8_t* data )
{
   // to avoid crashes, return 0 for channels, which do not exist
   if ( channel >= HmwChannel::getNumChannels() )
   {
      data[0] = 0;
      return 1;
   }
   return HmwChannel::getChannel( channel )->get( data );
}

void HmwDevice::set( uint8_t channel, uint8_t length, uint8_t const* const data )
{
   DEBUG_M3( FSTR( "SetC:" ), channel, ':' );
   for ( uint8_t i = 0; i < length; i++ )
   {
      DEBUG_L2( ' ', data[i] );
   }

   // to avoid crashes, do not try to set any channels, which do not exist
   if ( channel < HmwChannel::getNumChannels() )
   {
      HmwChannel::getChannel( channel )->set( length, data );
   }
}


#include <Peripherals/Oscillator.h>
#include <Peripherals/Clock.h>

static void
__attribute__( ( section( ".init3" ), naked, used ) )
lowLevelInit( void )
{
    #ifdef EIND
   __asm volatile ( "ldi r24,pm_hh8(__trampolines_start)\n\t"
                    "out %i0,r24" ::"n" ( &EIND ) : "r24", "memory" );
    #endif

    #ifdef DEBUG
   WatchDog::disable();
    #else
   WatchDog::enable( WatchDog::_4S );
    #endif


   Clock::configPrescalers( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );

   // Enable internal 32 MHz and 32kHz ring oscillator and wait until they are stable.
   Oscillator::enable( OSC_RC32MEN_bm | OSC_RC32KEN_bm );
   Oscillator::waitUntilOscillatorIsReady( OSC_RC32MEN_bm | OSC_RC32KEN_bm );

   // Set the 32 MHz ring oscillator as the main clock source.
   Clock::selectMainClockSource( CLK_SCLKSEL_RC32M_gc );
}

