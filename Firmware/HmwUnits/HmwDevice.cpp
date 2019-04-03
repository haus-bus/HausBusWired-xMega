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
#include "HmwMsgResetWifi.h"
#include "HmwMsgGetPacketSize.h"
#include "HmwMsgReadFlash.h"

#include <Peripherals/WatchDog.h>
#include <Peripherals/ResetSystem.h>


uint8_t HmwDevice::deviceType( 0 );

uint32_t HmwDevice::ownAddress( DEFAULT_ADDRESS );

HmwDeviceHw::BasicConfig* HmwDevice::basicConfig( 0 );

HmwDevice::PendingActions HmwDevice::pendingActions;

HmwDeviceHw* HmwDevice::hardware( NULL );

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
   handleConfigButton();
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
      WatchDog::reset();
      WatchDog::enable( WatchDog::_500MS );

      // wait for WatchDog
      while ( 1 )
      {
      }
   }
   if ( pendingActions.resetWifiConnection )
   {
      HmwMsgResetWifi msg( ownAddress, 2 );
      HmwStream::sendMessage( msg );
      pendingActions.resetWifiConnection = false;
   }
   if ( SystemTime::now() > FIRST_ANNOUNCEMENT_TIME )
   {
      handleAnnouncement();
   }
}

void HmwDevice::handleAnnouncement()
{
   if ( isAnnouncementPending() && HmwStream::isIdle() )
   {
      if ( announce() == IStream::SUCCESS )
      {
         clearPendingAnnouncement();
      }
   }
}

void HmwDevice::factoryReset()
{
   // clean up eeprom data but do not change hwVersion and ownAddress
   uint8_t _hwVersion = basicConfig->hwVersion;
   uint32_t _ownAddress = basicConfig->ownAddress;

   Eeprom::erase();

   // restore the data
   basicConfig->hwVersion = _hwVersion;
   basicConfig->ownAddress = _ownAddress;

   // reset the system after factory reset
   pendingActions.resetSystem = true;
}

void HmwDevice::handleConfigButton()
{
   // langer Tastendruck (5s) -> LED blinkt hektisch
   // dann innerhalb 10s langer Tastendruck (3s) -> LED geht aus, EEPROM-Reset

   // do we have a hardware associated?
   if ( !hardware )
   {
      return;
   }

   static Timestamp lastTime;
   static HmwDeviceHw::ConfigButtonState buttonState = HmwDeviceHw::IDLE;

   bool buttonPressed = hardware->isConfigButtonPressed();

   switch ( buttonState )
   {
      case HmwDeviceHw::IDLE:
      {
         if ( buttonPressed )
         {
            buttonState = HmwDeviceHw::FIRST_PRESS;
         }
         lastTime = Timestamp();
         break;
      }

      case HmwDeviceHw::FIRST_PRESS:
      {
         if ( buttonPressed )
         {  // button still pressed
            if ( lastTime.since() > 5000 )
            {
               buttonState = HmwDeviceHw::FIRST_LONG_PRESS;
            }
         }
         else
         {  // button released
            if ( lastTime.since() > 100 )
            {  // announce on short press
               pendingActions.announce = true;
            }
            buttonState = HmwDeviceHw::IDLE;
         }
         break;
      }

      case HmwDeviceHw::FIRST_LONG_PRESS:
      {
         if ( !buttonPressed )
         {  // button released
            buttonState = HmwDeviceHw::WAIT_SECOND_PRESS;
            lastTime = Timestamp();
            pendingActions.resetWifiConnection = true;
         }
         break;
      }

      case HmwDeviceHw::WAIT_SECOND_PRESS:
      {
         // debounce 100ms
         if ( lastTime.since() < 100 )
         {
            break;
         }
         if ( buttonPressed )
         {  // second button press
            buttonState = HmwDeviceHw::SECOND_PRESS;
            lastTime = Timestamp();
         }
         else
         {  // if second button press does not occur within 5s, continue with normal operation
            if ( lastTime.since() > 5000 )
            {
               buttonState = HmwDeviceHw::IDLE;
            }
         }
         break;
      }

      case HmwDeviceHw::SECOND_PRESS:
      {
         if ( lastTime.since() < 100 ) // entprellen
         {
            break;
         }
         if ( buttonPressed )
         {    // immer noch gedrueckt
            if ( lastTime.since() > 3000 )
            {
               buttonState = HmwDeviceHw::SECOND_LONG_PRESS;
            }
         }
         else
         {               // nicht mehr gedrueckt
            buttonState = HmwDeviceHw::IDLE;
         }
         break;
      }

      case HmwDeviceHw::SECOND_LONG_PRESS: // zweiter Druck erkannt
      {
         if ( !buttonPressed )
         {  // erst wenn losgelassen
            // Factory-Reset
            factoryReset();
            buttonState = HmwDeviceHw::IDLE;
         }
         break;
      }
   }
   hardware->notifyConfigButtonState( buttonState );
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

   if ( msg.isCommand( HmwMessageBase::START_BOOTER ) )
   {
      DEBUG_M1( FSTR( "C: START_BOOTER" ) );
      pendingActions.startBooter = true;
   }
   else if ( msg.isCommand( HmwMessageBase::READ_EEPROM ) )
   {
      DEBUG_M1( FSTR( "C: READ_EEPROM" ) );
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

#ifdef _BOOTER_
   else if ( msg.isCommand( HmwMessageBase::START_FW ) )
   {
      DEBUG_M1( FSTR( "C: START_FW" ) );
      pendingActions.startFirmware = true;
   }
   else if ( msg.isCommand( HmwMessageBase::GET_PACKET_SIZE ) )
   {
      DEBUG_M1( FSTR( "C: GET_PACKET_SIZE" ) );
      ( (HmwMsgGetPacketSize*)&msg )->setupResponse();
      ackOnly = false;
   }
   else if ( msg.isCommand( HmwMessageBase::WRITE_FLASH ) )
   {
      DEBUG_M1( FSTR( "C:WRITE_FLASH" ) );
      if ( msg.getFrameDataLength() >= 4 )
      {
         HmwMsgWriteFlash* msgWriteFlash = (HmwMsgWriteFlash*)&msg;
         Flash::address_t address = msgWriteFlash->getAddress();
         uint8_t length = msgWriteFlash->getLength();

         if ( Flash::write( address, msgWriteFlash->getData(), length ) != length )
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
   else if ( msg.isCommand( HmwMessageBase::READ_FLASH ) )
   {
      DEBUG_M1( FSTR( "C: READ_FLASH" ) );
      if ( msg.getFrameDataLength() == 4 )
      {
         ( (HmwMsgReadFlash*)&msg )->setupResponse();
         ackOnly = false;
      }
      else
      {
         DEBUG_M2( FSTR( "E: wrong data length :" ), msg.getFrameDataLength() );
         isValid = false;
      }
   }
#else
   else if ( msg.isCommand( HmwMessageBase::READ_CONFIG ) )
   {
      DEBUG_M1( FSTR( "C: READ_CONFIG" ) );
      pendingActions.readConfig = true;
   }
   else if ( msg.isCommand( HmwMessageBase::GET_FW_VERSION ) )
   {
      DEBUG_M1( FSTR( "C: GET_FW_VERSION" ) );
      // this command is allowed to be broadcast and should be handled as a non broadcast
      // This is needed by the Loxone bridge from HAUS-BUS.de
      msg.setTargetAddress( ownAddress );
      ( (HmwMsgGetFwVersion*)&msg )->setupResponse( ( Release::MAJOR << 8 ) | Release::MINOR );
      ackOnly = false;
   }
   else if ( msg.isCommand( HmwMessageBase::GET_HARDWARE_VERSION ) )
   {
      DEBUG_M1( FSTR( "C: HWVer,Typ" ) );
      ( (HmwMsgGetHwVersion*)&msg )->setupResponse( HmwDevice::deviceType, basicConfig->hwVersion );
      ackOnly = false;
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
      HmwMsgKeyEvent* event = ( HmwMsgKeyEvent* )&msg;

      // forward all key events to LinkReceiver except the short pressed broadcasts
      if ( !( !event->isLongPress() && msg.isBroadcast() ) )
      {
         DEBUG_L1( FSTR( "->LinkReceiver" ) )
         HmwLinkReceiver::notifyKeyEvent( event->getSenderAddress(), event->getSourceChannel(), event->getDestinationChannel(), event->isLongPress(), event->getKeyPressNum() );
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
   else if ( msg.isCommand( HmwMessageBase::START_ZERO_COMMUNICATION ) )
   {
      DEBUG_M1( FSTR( "C: START_ZERO_COMMUNICATION" ) );
      pendingActions.zeroCommunicationActive = true;
   }
   else if ( msg.isCommand( HmwMessageBase::END_ZERO_COMMUNICATION ) )
   {
      DEBUG_M1( FSTR( "C: END_ZERO_COMMUNICATION" ) );
      pendingActions.zeroCommunicationActive = false;
   }
#endif

   else
   {
      return false;
   }

   if ( isValid
      && !msg.isBroadcast()
      && ( !pendingActions.zeroCommunicationActive || msg.isCommand( HmwMessageBase::START_BOOTER ) ) ) // START_BOOTER is the only command that allows a response during z-Mode
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

