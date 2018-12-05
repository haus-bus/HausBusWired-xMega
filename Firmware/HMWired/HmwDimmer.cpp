/*
 * HBWDimmer.cpp
 *
 * Created: 26.04.2017 09:01:56
 * Author: viktor.pankraz
 */


#include "HmwDimmer.h"
#include "HmwDevice.h"

HmwDimmer::HmwDimmer( PortPin _portPin, Config* _config ) :
   pwmOutput( _portPin.getPortNumber(), _portPin.getPinNumber() )
{
   type = HmwChannel::HMW_DIMMER;
   config = _config;
   pwmOutput.setInverted( config->getDimmingMode() == Config::DIMM_L );
   currentLevel = 0;
   nextFeedbackTime.reset();
}


void HmwDimmer::set( uint8_t length, uint8_t const* const data )
{
   if ( *data <= MAX_LEVEL )
   {
      currentLevel = *data;
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
   }

   // Logging
   if ( !nextFeedbackTime.isValid() && config->isLogging() )
   {
      nextFeedbackTime = Timestamp();
      nextFeedbackTime += ( HmwDevice::getLoggingTime() * 100 );
   }
}


uint8_t HmwDimmer::get( uint8_t* data )
{
   // map 0-100% to 0-200
   ( *data ) = currentLevel;
   return 1;
}

void HmwDimmer::loop( uint8_t channel )
{

    // the default range is 0-200, this must be mapped to 0-100% duty cycle
    setLevel( currentLevel );

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

void HmwDimmer::checkConfig()
{
   pwmOutput.setInverted( config->getDimmingMode() == Config::DIMM_L );
}

void HmwDimmer::setLevel( uint8_t level )
{
   // special function for Config::levelFactor == 0, no PWM
   if ( getLevel() != level )
   {
      pwmOutput.set( level * NORMALIZE_LEVEL );
   }
}

uint8_t HmwDimmer::getLevel() const
{
   // normalize to 0-200 
   return pwmOutput.isSet() / NORMALIZE_LEVEL;
}