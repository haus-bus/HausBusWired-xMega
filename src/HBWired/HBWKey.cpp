#include "HBWKey.h"

// Class HBWKey
HBWKey::HBWKey( PortPin _pin, Config* _config, HBWChannel* _feedbackChannel, bool _inverted ) : digitalIn( _pin )
{
   type = HBWChannel::HBW_KEY;
   digitalIn.enablePullup();
   digitalIn.setInverted( _inverted );
   keyPressedMillis = 0;
   keyPressNum = 0;
   config = _config;
   feedbackChannel = _feedbackChannel;
}


void HBWKey::loop( HBWDevice* device, uint8_t channel )
{
   if ( config->isUnlocked() )
   {
      uint32_t now = SystemTime::now();
      if ( digitalIn.isSet() )
      {
         // d.h. Taste nicht gedrueckt
         // "Taste war auch vorher nicht gedrueckt" kann ignoriert werden
         // Taste war vorher gedrueckt?
         if ( keyPressedMillis )
         {
            // entprellen, nur senden, wenn laenger als 50ms gedrueckt
            // aber noch kein "long" gesendet
            if ( now - keyPressedMillis >= 50 && !lastSentLong )
            {
               keyPressNum++;
               device->sendKeyEvent( channel, keyPressNum, false );
            }
            keyPressedMillis = 0;
            if ( feedbackChannel )
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
         if ( keyPressedMillis )
         {
            // muessen wir ein "long" senden?
            if ( lastSentLong )
            {       // schon ein LONG gesendet
               if ( now - lastSentLong >= 300 )
               {       // alle 300ms wiederholen
                  // keyPressNum nicht erhoehen
                  lastSentLong = now ? now : 1;                           // der Teufel ist ein Eichhoernchen
                  device->sendKeyEvent( channel, keyPressNum, true );             // long press
               }
            }
            else if ( now - keyPressedMillis >= long(config->getLongPressTime() ) * 100 )
            {
               // erstes LONG
               keyPressNum++;
               lastSentLong = now ? now : 1;
               device->sendKeyEvent( channel, keyPressNum, true );                // long press
            }
         }
         else
         {
            // Taste war vorher nicht gedrueckt
            keyPressedMillis = now ? now : 1;                 // der Teufel ist ein Eichhoernchen
            lastSentLong = 0;
            if ( feedbackChannel )
            {
               uint8_t cmd = KEY_FEEDBACK_ON;
               feedbackChannel->set( device, 1, &cmd );
            }
         }
      }
   }
}

void HBWKey::checkConfig()
{
   if ( ( config->getLongPressTime() < 4 ) || ( config->getLongPressTime() > 50 ) )
   {
      config->setLongPressTime( 10 );
   }
}