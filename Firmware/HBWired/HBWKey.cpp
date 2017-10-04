#include "HBWKey.h"

// Class HBWKey
HBWKey::HBWKey( PortPin _pin, Config* _config, HBWChannel* _feedbackChannel ) : digitalIn( _pin )
{
   type = HBWChannel::HBW_KEY;
   config = _config;
   feedbackChannel = _feedbackChannel;
   resetChannel();
}


void HBWKey::loop( HBWDevice* device, uint8_t channel )
{
   if ( config->isUnlocked() )
   {
      if ( config->isPushButton() )
      {
         handlePushButtonSignal( device, channel );
      }
      else if ( config->isSwitch() )
      {
         handleSwitchSignal( device, channel );
      }
   }
}

void HBWKey::handleSwitchSignal( HBWDevice* device, uint8_t channel )
{
   if ( digitalIn.isSet() )
   {
      if ( lastSentLong.isValid() )
      {

         // if return value is 1, bus is not idle, retry next time
         if ( device->sendKeyEvent( channel, keyPressNum, false ) != 1 )
         {
            keyPressNum++;
            lastSentLong.reset();
         }
      }
      keyPressedTimestamp.reset();
      if ( feedbackChannel && config->isFeedbackEnabled() )
      {
         uint8_t cmd = KEY_FEEDBACK_OFF;
         feedbackChannel->set( device, 1, &cmd );
      }
   }
   else
   {
      if ( !keyPressedTimestamp.isValid() )
      {
         // Taste war vorher nicht gedrueckt
         keyPressedTimestamp = Timestamp();
      }
      else if ( ( keyPressedTimestamp.since() >= 100 ) && !lastSentLong.isValid() )
      {
         // if return value is 1, bus is not idle, retry next time
         if ( device->sendKeyEvent( channel, keyPressNum, false ) != 1 )
         {
            keyPressNum++;
            lastSentLong = Timestamp();
         }
      }
      if ( feedbackChannel && config->isFeedbackEnabled() )
      {
         uint8_t cmd = KEY_FEEDBACK_ON;
         feedbackChannel->set( device, 1, &cmd );
      }
   }
}

void HBWKey::handlePushButtonSignal( HBWDevice* device, uint8_t channel )
{
   if ( digitalIn.isSet() )
   {
      // d.h. Taste nicht gedrueckt
      // "Taste war auch vorher nicht gedrueckt" kann ignoriert werden
      // Taste war vorher gedrueckt?
      if ( keyPressedTimestamp.isValid() )
      {
         // entprellen, nur senden, wenn laenger als 50ms gedrueckt
         // aber noch kein "long" gesendet
         if ( ( keyPressedTimestamp.since() >= 50 ) && !lastSentLong.isValid() )
         {
            keyPressNum++;
            device->sendKeyEvent( channel, keyPressNum, false );
         }
         keyPressedTimestamp.reset();
         if ( feedbackChannel && config->isFeedbackEnabled() )
         {
            uint8_t cmd = KEY_FEEDBACK_OFF;
            feedbackChannel->set( device, 1, &cmd );
         }
      }
   }
   else
   {
      // Taste gedrueckt
      // Taste war vorher schon gedrueckt
      if ( keyPressedTimestamp.isValid() )
      {
         // muessen wir ein "long" senden?
         if ( lastSentLong.isValid() )
         {          // schon ein LONG gesendet
            if ( lastSentLong.since() >= 300 )
            {           // alle 300ms wiederholen
               // keyPressNum nicht erhoehen
               lastSentLong = Timestamp();
               device->sendKeyEvent( channel, keyPressNum, true );                  // long press
            }
         }
         else if ( keyPressedTimestamp.since() >= long(config->getLongPressTime() ) * 100 )
         {
            // erstes LONG
            keyPressNum++;
            lastSentLong = Timestamp();
            device->sendKeyEvent( channel, keyPressNum, true );                    // long press
         }
      }
      else
      {
         // Taste war vorher nicht gedrueckt
         keyPressedTimestamp = Timestamp();
         lastSentLong.reset();
         if ( feedbackChannel && config->isFeedbackEnabled() )
         {
            uint8_t cmd = KEY_FEEDBACK_ON;
            feedbackChannel->set( device, 1, &cmd );
         }
      }
   }
}

void HBWKey::resetChannel()
{
   if ( config->isActiveLow() )
   {
      digitalIn.setInverted( false );
      digitalIn.enablePullup();
   }
   else
   {
      digitalIn.setInverted( true );
      digitalIn.enablePulldown();
   }
   keyPressedTimestamp.reset();
   lastSentLong.reset();
   keyPressNum = 0;
}

void HBWKey::checkConfig()
{
   if ( ( config->getLongPressTime() < 4 ) || ( config->getLongPressTime() > 50 ) )
   {
      config->setLongPressTime( 10 );
   }
   if ( config->getInputType() >= Config::MAX_INPUTTYPE )
   {
      config->setInputType( Config::PUSHBUTTON );
   }
   resetChannel();
}