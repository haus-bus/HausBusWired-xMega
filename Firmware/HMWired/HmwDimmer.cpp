/*
 * HBWDimmer.cpp
 *
 * Created: 26.04.2017 09:01:56
 * Author: viktor.pankraz
 */


#include "HmwDimmer.h"
#include "HmwDevice.h"

#define getId() FSTR( "HmwDimmer." ) << channelId

const uint8_t HmwDimmer::debugLevel( DEBUG_LEVEL_LOW | DEBUG_STATE_L3 );

HmwDimmer::HmwDimmer( PortPin _portPin, PortPin _enablePin, Config* _config, uint8_t _normalizeLevel ) :
   normalizeLevel( _normalizeLevel ),
   pwmOutput( _portPin.getPortNumber(), _portPin.getPinNumber() ),
   enableOutput( _enablePin ),
   config( _config ),
   actionParameter( NULL ),
   nextFeedbackTime( 0 ),
   nextActionTime( 0 )
{
   type = HmwChannel::HMW_DIMMER;
   SET_STATE_L1( OFF );
   // Todo pwmOutput.setInverted( config->getDimmingMode() == Config::DIMM_L );
}


void HmwDimmer::set( uint8_t length, uint8_t const* const data )
{
   if ( ( length == 1 ) && ( *data <= MAX_LEVEL ) )
   {
      setLevel( *data );
      nextActionTime.reset();
      if ( *data )
      {
         SET_STATE_L1( ON );
      }
      else
      {
         SET_STATE_L1( OFF );
      }
   }
   else if ( length == sizeof( ActionParameter ) )
   {
      actionParameter = (ActionParameter const*)data;
      switch ( actionParameter->actionType )
      {
         case JUMP_TO_TARGET:
         {
            handleJumpToTargetCmd();
            break;
         }
         default:
         {
            WARN_3( FSTR( "HmwDimmer::set actionType: 0x" ), (uint8_t)actionParameter->actionType, FSTR( " not implemented" ) );
         }
      }
   }
   else
   {
      ERROR_2( FSTR( "HmwDimmer::set unknown command length: 0x" ), length );
   }
}


uint8_t HmwDimmer::get( uint8_t* data )
{
   // map 0-100% to 0-200
   ( *data ) = getLevel();
   return 1;
}

void HmwDimmer::loop( uint8_t channel )
{
   if ( nextActionTime.isValid() && nextActionTime.since() )
   {
      handleStateChart();
   }

   // feedback trigger set?
   if ( !nextFeedbackTime.isValid() || !nextFeedbackTime.since() )
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
   config->checkOrRestore();
   // Todo pwmOutput.setInverted( config->getDimmingMode() == Config::DIMM_L );
}

void HmwDimmer::setLevel( uint8_t level )
{
   // special function for Config::levelFactor == 0, no PWM
   if ( getLevel() != level )
   {
      DEBUG_H2( FSTR( " setLevel 0x" ), level );
      level ? enableOutput.set() : enableOutput.clear();
      pwmOutput.set( level * normalizeLevel );

      // Logging
      if ( !nextFeedbackTime.isValid() && config->isLogging() )
      {
         nextFeedbackTime = Timestamp();
         nextFeedbackTime += ( HmwDevice::getLoggingTime() * 100 );
      }
   }
}

uint8_t HmwDimmer::getLevel() const
{
   if ( pwmOutput.isRunning() )
   {
      // normalize to 0-200
      return pwmOutput.isSet() / normalizeLevel;
   }
   return pwmOutput.isSet() ? MAX_LEVEL : 0;
}

void HmwDimmer::handleStateChart( bool fromMainLoop )
{
   if ( actionParameter == NULL )
   {
      return;
   }
   switch ( state )
   {
      case TIME_OFF:
      case OFF:
      {
         if ( isValidActionTime( actionParameter->onDelayTime ) )
         {
            SET_STATE_L1( DELAY_ON );
            nextActionTime = Timestamp();
            nextActionTime += actionParameter->onDelayTime * SystemTime::S / 10;
         }
         else
         {
            SET_STATE_L1( RAMP_UP );
            nextActionTime = Timestamp();
            calculateRampParameter();
         }
         break;
      }
      case DELAY_ON:
      {
         if ( fromMainLoop )
         {
            SET_STATE_L1( RAMP_UP );
            nextActionTime = Timestamp();
            calculateRampParameter();
            break;
         }
      }
      case RAMP_UP:
      {
         uint8_t currentLevel = getLevel();
         if ( !fromMainLoop || ( currentLevel >= actionParameter->onLevel ) )
         {
            if ( isValidActionTime( actionParameter->onTime ) )
            {
               SET_STATE_L1( TIME_ON );
               nextActionTime = Timestamp();
               nextActionTime += actionParameter->onTime * SystemTime::S / 10;
            }
            else
            {
               SET_STATE_L1( ON );
               nextActionTime.reset();
            }
            setLevel( actionParameter->onLevel );
         }
         else
         {
            // do the ramp
            if ( (uint16_t)( currentLevel + rampStep ) >= actionParameter->onLevel )
            {
               setLevel( actionParameter->onLevel );
            }
            else
            {
               setLevel( currentLevel + rampStep );
               nextActionTime += ( nextStepTime );
            }
         }

         break;
      }
      case TIME_ON:
      case ON:
      {
         if ( actionParameter->offDelayTime < MAX_NEXT_ACTION_TIME )
         {
            SET_STATE_L1( DELAY_OFF );
            nextActionTime = Timestamp();
            nextActionTime += actionParameter->offDelayTime * SystemTime::S / 10;
         }
         else
         {
            SET_STATE_L1( RAMP_DOWN );
            nextActionTime = Timestamp();
            calculateRampParameter();
         }
         break;
      }
      case DELAY_OFF:
      {
         if ( fromMainLoop )
         {
            SET_STATE_L1( RAMP_DOWN );
            nextActionTime = Timestamp();
            calculateRampParameter();
            break;
         }
      }
      case RAMP_DOWN:
      {
         uint8_t currentLevel = getLevel();
         if ( !fromMainLoop || ( currentLevel <= actionParameter->offLevel ) )
         {
            if ( isValidActionTime( actionParameter->offTime ) )
            {
               SET_STATE_L1( TIME_OFF );
               nextActionTime = Timestamp();
               nextActionTime += actionParameter->offTime * SystemTime::S / 10;
            }
            else
            {
               SET_STATE_L1( OFF );
               nextActionTime.reset();
            }
            setLevel( actionParameter->offLevel );
         }
         else
         {
            // do the ramp
            if ( ( currentLevel <= rampStep ) || ( ( currentLevel - rampStep ) <= actionParameter->offLevel ) )
            {
               setLevel( actionParameter->offLevel );
            }
            else
            {
               setLevel( currentLevel - rampStep );
               nextActionTime += ( nextStepTime );
            }
         }
         break;
      }
      default:
      {
         WARN_3( FSTR( "HmwDimmer::handleJumpToTargetCmd from state: 0x" ), (uint8_t)state, FSTR( " not implemented" ) );
      }
   }
}

void HmwDimmer::calculateRampParameter()
{
   if ( ( state != RAMP_UP ) && ( state != RAMP_DOWN ) )
   {
      ERROR_3( FSTR( "HmwDimmer::calculateRampParameter for state: 0x" ), (uint8_t)state, FSTR( " not implemented" ) );
      return;
   }
   DEBUG_H1( FSTR( " calculateRampParameter" ) );
   rampStep = actionParameter->onLevel - actionParameter->offLevel;
   nextStepTime = actionParameter->rampOnTime;

   if ( state == RAMP_DOWN )
   {
      nextStepTime = actionParameter->rampOffTime;
   }
   nextStepTime = nextStepTime * SystemTime::S / 10;

   DEBUG_M2( FSTR( "total Steps : 0x" ), rampStep );
   DEBUG_M2( FSTR( "total Time : 0x" ), nextStepTime );

   if ( nextStepTime >= ( rampStep * LOOP_PERIOD_MS ) )
   {
      nextStepTime /= rampStep;
      rampStep = 1;
   }
   else
   {
      while ( ( nextStepTime >= ( 2 * LOOP_PERIOD_MS ) ) && ( rampStep >= actionParameter->dimStep ) )
      {
         rampStep /= 2;
         nextStepTime /= 2;
      }
   }

   DEBUG_M2( FSTR( "rampStep : 0x" ), rampStep );
   DEBUG_M2( FSTR( "stepTime : 0x" ), nextStepTime );
}

void HmwDimmer::handleJumpToTargetCmd()
{
   handleStateChart( false );
}