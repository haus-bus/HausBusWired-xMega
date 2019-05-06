/*
 * HBWLed.cpp
 *
 * Created: 26.04.2017 09:01:56
 * Author: viktor.pankraz
 */


#include "HmwLed.h"
#include "HmwDevice.h"

HmwLed::HmwLed( PortPin _portPin, Config* _config, bool _inverted, uint8_t _defaultPwmRange ) :
   pwmOutput( _portPin.getPortNumber(), _portPin.getPinNumber(), MAX_LEVEL_PERIOD ),
   defaultPwmRange( _defaultPwmRange )
{
   type = HmwChannel::HMW_LED;
   config = _config;
   pwmOutput.setInverted( _inverted );
   pwmOutput.clear();
   feedbackCmdActive = false;
   logicalState = OFF;
   currentLevel = 0;
   onLevel = MAX_LEVEL;
   offLevel = 0;
   blinkOnTime = 10;
   blinkOffTime = 10;
   blinkQuantity = 255;
   nextFeedbackTime.reset();
   nextBlinkTime.reset();
}


void HmwLed::set( uint8_t length, uint8_t const* const data )
{
   if ( *data <= MAX_LEVEL )
   {
      currentLevel = *data;
      nextBlinkTime.reset();
      setLogicalState( *data ? ON : OFF );
   }
   else if ( isKeyFeedbackOnCmd( *data ) )
   {
      setLevel( MAX_LEVEL );
      feedbackCmdActive = true;
      return;   // no logging for feedbackCmd
   }
   else if ( isKeyFeedbackOffCmd( *data ) )
   {
      feedbackCmdActive = false;
      return;   // no logging for feedbackCmd
   }
   else if ( length >= 6 )
   {
      offLevel = data[1];
      onLevel = data[2];
      blinkOnTime = data[3];
      blinkOffTime = data[4];
      blinkQuantity = data[5];

      if ( isBlinkOnCmd( *data ) )
      {
         nextBlinkTime = Timestamp();
         setLogicalState( BLINK_ON );
      }
      else if ( isBlinkToggleCmd( *data ) )
      {
         if ( logicalState != BLINK_ON )
         {
            nextBlinkTime = Timestamp();
            setLogicalState( BLINK_ON );
         }
         else
         {
            nextBlinkTime.reset();
            setLogicalState( OFF );
         }
      }
      else if ( isToggleCmd( *data ) )
      {
         if ( isLogicalOn() )
         {
            currentLevel = offLevel;
            setLogicalState( OFF );
         }
         else
         {
            currentLevel = onLevel;
            setLogicalState( ON );
         }
         nextBlinkTime.reset();
      }
   }
   else  // toggle
   {
      if ( currentLevel )
      {
         currentLevel = 0;
      }
      else
      {
         currentLevel = MAX_LEVEL;
      }
      nextBlinkTime.reset();
   }

   // Logging
   if ( !nextFeedbackTime.isValid() && config->isLogging() )
   {
      nextFeedbackTime = Timestamp();
      nextFeedbackTime += ( HmwDevice::getLoggingTime() * 100 );
   }
}


uint8_t HmwLed::get( uint8_t* data )
{
   // map 0-100% to 0-200
   ( *data ) = currentLevel;
   return 1;
}

void HmwLed::loop( uint8_t channel )
{
   if ( nextBlinkTime.isValid() && nextBlinkTime.since() )
   {
      // handle blinking
      if ( getLevel() == onLevel )
      {
         // is ON
         nextBlinkTime += ( blinkOffTime * 100 );
         setLevel( offLevel );
      }
      else
      {
         // is OFF
         if ( blinkQuantity )
         {
            nextBlinkTime += ( blinkOnTime * 100 );
            setLevel( onLevel );

            if ( blinkQuantity != 255 )
            {
               blinkQuantity--;
            }
         }
         else
         {
            nextBlinkTime.reset();
            setLogicalState( currentLevel == onLevel ? ON : OFF );
         }
      }

   }
   if ( !feedbackCmdActive && !nextBlinkTime.isValid() )
   {
      // the default range is 0-200, this must be mapped to 0-100% duty cycle
      setLevel( currentLevel );
   }

   // feedback trigger set?
   if ( !nextFeedbackTime.isValid() )
   {
      return;
   }

   if ( !nextFeedbackTime.since() )
   {
      return;
   }

   uint8_t level;
   uint8_t errcode = HmwDevice::sendInfoMessage( channel, get( &level ), &level );

   // sendInfoMessage returns 0 on success, 1 if bus busy, 2 if failed
   if ( errcode )
   {
      // bus busy
      // try again later, but insert a small delay
      nextFeedbackTime += 250;
   }
   else
   {
      nextFeedbackTime.reset();
   }
}

void HmwLed::checkConfig()
{
   if ( config->getPwmRange() > 100 )
   {
      config->setPwmRange( defaultPwmRange );
   }
}

void HmwLed::setLevel( uint8_t level )
{
   // special function for Config::levelFactor == 0, no PWM
   if ( config->getPwmRange() )
   {
      pwmOutput.set( level * config->getPwmRange() / NORMALIZE_LEVEL );
   }
   else
   {
      pwmOutput.set( level ? MAX_LEVEL_PERIOD : 0 );
   }
}

uint8_t HmwLed::getLevel() const
{
   // special function for Config::levelFactor == 0, no PWM
   if ( config->getPwmRange() )
   {
      return pwmOutput.isSet() * NORMALIZE_LEVEL / config->getPwmRange();
   }
   return pwmOutput.isSet() ? MAX_LEVEL : 0;
}