/*
 * HBWDimmer.cpp
 *
 * Created: 26.04.2017 09:01:56
 * Author: viktor.pankraz
 */


#include "HmwDimmer.h"
#include "HmwDevice.h"

#define getId() FSTR( "HmwDimmer." ) << channelId

const uint8_t HmwDimmer::debugLevel( DEBUG_LEVEL_OFF | DEBUG_STATE_L3 );

HmwDimmer::HmwDimmer( PortPin _portPin, PortPin _enablePin, Config* _config, uint8_t _normalizeLevel ) :
   normalizeLevel( _normalizeLevel ),
   dimmingFactor( _normalizeLevel ),
   dimmingOffset( 0 ),
   pwmOutput( _portPin.getPortNumber(), _portPin.getPinNumber() ),
   enableOutput( _enablePin ),
   currentLevel( 0 ),
   config( _config ),
   actionParameter( NULL ),
   nextActionTime( 0 ),
   lastKeyNum( 0 )
{
   type = HmwChannel::HMW_DIMMER;
   pwmOutput.setInverted( config->isDimmingModeLeading() );
   calculateDimmingParameter();
   SET_STATE_L1( START_UP );
}


void HmwDimmer::set( uint8_t length, uint8_t const* const data )
{
   if ( ( length == 1 ) && ( *data <= MAX_LEVEL ) )
   {
      setLevel( *data );
      nextActionTime.reset();
      if ( *data )
      {
         SET_STATE_L1( JT_ON );
      }
      else
      {
         SET_STATE_L1( JT_OFF );
      }
   }
   else if ( length == sizeof( LinkCommand ) )
   {
      LinkCommand const* cmd = (LinkCommand const*)data;
      actionParameter = cmd->actionParameter;
      /*
         DEBUG_H2( FSTR( "setAction 0x" ), (uintptr_t)actionParameter );
         DEBUG_M2( FSTR( "onTimeMode   " ), actionParameter->onTimeMode );
         DEBUG_M2( FSTR( "offTimeMode  " ), actionParameter->offTimeMode );
         DEBUG_M2( FSTR( "onDelayMode  " ), actionParameter->onDelayMode );
         DEBUG_M2( FSTR( "multiExecute " ), actionParameter->multiExecute );
         DEBUG_M2( FSTR( "actionType   " ), actionParameter->actionType );
         DEBUG_M2( FSTR( "offLevel     " ), actionParameter->offLevel );
         DEBUG_M2( FSTR( "onMinLevel   " ), actionParameter->onMinLevel );
         DEBUG_M2( FSTR( "onLevel      " ), actionParameter->onLevel );
         DEBUG_M2( FSTR( "rampStartStep" ), actionParameter->rampStartStep );
         DEBUG_M2( FSTR( "offDelayStep " ), actionParameter->offDelayStep );
         DEBUG_M2( FSTR( "onDelayTime  " ), actionParameter->onDelayTime );
         DEBUG_M2( FSTR( "rampOnTime   " ), actionParameter->rampOnTime );
         DEBUG_M2( FSTR( "onTime       " ), actionParameter->onTime );
         DEBUG_M2( FSTR( "offDelayTime " ), actionParameter->offDelayTime );
         DEBUG_M2( FSTR( "rampOffTime  " ), actionParameter->rampOffTime );
         DEBUG_M2( FSTR( "offTime      " ), actionParameter->offTime );
         DEBUG_M2( FSTR( "dimMinLevel  " ), actionParameter->dimMinLevel );
         DEBUG_M2( FSTR( "dimMaxLevel  " ), actionParameter->dimMaxLevel );
         DEBUG_M2( FSTR( "dimStep      " ), actionParameter->dimStep );
         DEBUG_M2( FSTR( "jtRampOn     " ), actionParameter->jtRampOn );
         DEBUG_M2( FSTR( "jtOnDelay    " ), actionParameter->jtOnDelay );
         DEBUG_M2( FSTR( "jtOffDelay   " ), actionParameter->jtOffDelay );
         DEBUG_M2( FSTR( "jtOn         " ), actionParameter->jtOn );
         DEBUG_M2( FSTR( "jtOff        " ), actionParameter->jtOff );
         DEBUG_M2( FSTR( "jtRampOff    " ), actionParameter->jtRampOff );
       */
      if ( actionParameter->actionType > JUMP_TO_TARGET )
      {
         // do not interrupt running timer. First key press goes here, repeated press only when LONG_MULTIEXECUTE is enabled
         if ( !nextActionTime.isValid() && ( ( lastKeyNum != cmd->keyNum ) || ( lastKeyNum == cmd->keyNum && actionParameter->multiExecute ) ) )
         {
            uint8_t level = -1;
            switch ( actionParameter->actionType )
            {
               case TOGGLE_TO_COUNTER:
               {
                  level = ( cmd->keyNum & 1 ) ? actionParameter->onLevel : actionParameter->offLevel; // switch ON at odd numbers, OFF at even numbers
                  break;
               }
               case TOGGLE_INVERS_TO_COUNTER:
               {
                  level = ( cmd->keyNum & 1 ) ? actionParameter->offLevel : actionParameter->onLevel; // switch OFF at odd numbers, ON at even numbers
                  break;
               }
               case UPDIM:
               {
                  level = getNextDimLevel( true );
                  break;
               }
               case DOWNDIM:
               {
                  level = getNextDimLevel( false );
                  break;
               }
               case TOGGLEDIM:
               case TOGGLEDIM_TO_COUNTER:
               {
                  level = getNextDimLevel( cmd->keyNum & 1 );
                  break;
               }
               case TOGGLEDIM_INVERS_TO_COUNTER:
               {
                  level = getNextDimLevel( !( cmd->keyNum & 1 ) );
                  break;
               }
               default:
               {
                  WARN_3( FSTR( "HmwDimmer::set actionType: 0x" ), (uint8_t)actionParameter->actionType, FSTR( " not implemented" ) );
               }
            }

            if ( level <= MAX_LEVEL )
            {
               setLevel( level );
               nextActionTime.reset();
               if ( level )
               {
                  SET_STATE_L1( JT_ON );
               }
               else
               {
                  SET_STATE_L1( JT_OFF );
               }
            }
         }
      }
      else if ( lastKeyNum == cmd->keyNum && !actionParameter->multiExecute )
      {
         // repeated key event for ACTION_TYPE == 1 (ACTION_TYPE == 0 already filtered by receiveKeyEvent, HBWLinkReceiver)
         // must be long press, but LONG_MULTIEXECUTE not enabled
      }
      else if ( actionParameter->actionType == JUMP_TO_TARGET )
      {
         handleStateChart( false );
      }
      lastKeyNum = cmd->keyNum;  // store key press number, to identify repeated key events
   }
   else
   {
      ERROR_2( FSTR( "HmwDimmer::set unknown command length: 0x" ), length );
   }
}

uint8_t HmwDimmer::getNextDimLevel( bool dimUp )
{

   uint16_t level = currentLevel;  // keep current level if we cannot dim up or down anymore

   if ( dimUp )
   {
      if ( currentLevel < actionParameter->dimMaxLevel )
      {
         level += ( actionParameter->dimStep * 2 );
         if ( level >= actionParameter->dimMaxLevel )
         {
            return actionParameter->dimMaxLevel;
         }
         if ( ( level > actionParameter->offLevel ) && ( level < actionParameter->onMinLevel ) )
         {
            return actionParameter->onMinLevel;
         }
      }
      else
      {
         return actionParameter->dimMaxLevel;
      }
   }
   else
   {
      if ( currentLevel > actionParameter->dimMinLevel )
      {
         level -= ( actionParameter->dimStep * 2 );
         if ( level <= actionParameter->dimMinLevel )
         {
            return actionParameter->dimMinLevel;
         }
         if ( level < actionParameter->onMinLevel )
         {
            return actionParameter->onMinLevel;
         }
      }
      else
      {
         return actionParameter->dimMinLevel;
      }
   }

   return (uint8_t)level;
}


uint8_t HmwDimmer::get( uint8_t* data )
{
   // map 0-100% to 0-200
   ( *data ) = currentLevel;
   return 1;
}

void HmwDimmer::loop( uint8_t channel )
{
   if ( ( nextActionTime.isValid() && nextActionTime.since() ) || ( state == START_UP ) )
   {
      handleStateChart();
   }
   handleFeedback();
}

void HmwDimmer::checkConfig()
{
   config->checkOrRestore();
   pwmOutput.setInverted( config->isDimmingModeLeading() );
   calculateDimmingParameter();
}

void HmwDimmer::setLevel( uint8_t level )
{
   if ( ( currentLevel != level ) || ( state == START_UP ) )
   {
      DEBUG_H2( FSTR( " setLevel 0x" ), level );
      level ? enableOutput.set() : enableOutput.clear();
      currentLevel = level;

      // handle first all the special cases, where PWM is not needed
      // dimming mode PWM is currently not supported and will be handled like SWITCH mode
      if ( config->isDimmingModeSwitch() || config->isDimmingModePwm() )
      {
         // disable PWM in this mode, set only digital pin
         level ? pwmOutput.DigitalOutput::set() : pwmOutput.clear();
      }
      else
      {
         if ( config->isDimmingModeLeading() )
         {
            level = MAX_LEVEL - level;
         }
         if ( level )
         {
            uint16_t pwmValue = level * dimmingFactor + dimmingOffset;
            pwmOutput.set( pwmValue );
            DEBUG_M2( FSTR( " pwmValue 0x" ), pwmValue );
         }
         else
         {
            pwmOutput.clear();
         }
      }

      checkLogging( config->isLogging() );
   }
}

void HmwDimmer::jumpToNextState( uint8_t nextState )
{
   nextActionTime = Timestamp();

   if ( JT_ONDELAY == nextState )
   {
      if ( isValidActionTime( actionParameter->onDelayTime ) )
      {
         nextActionTime += convertToTime( actionParameter->onDelayTime );
         SET_STATE_L1( JT_ONDELAY );
      }
      else
      {
         jumpToNextState( JT_RAMP_ON );
      }
   }
   else if ( JT_RAMP_ON == nextState )
   {
      if ( isValidActionTime( actionParameter->rampOnTime ) )
      {
         SET_STATE_L1( JT_RAMP_ON );
         calculateRampParameter();
      }
      else
      {
         jumpToNextState( JT_ON );
      }
   }
   else if ( JT_ON == nextState )
   {
      SET_STATE_L1( JT_ON );
      if ( isValidActionTime( actionParameter->onTime ) )
      {
         nextActionTime += convertToTime( actionParameter->onTime );
      }
      else
      {
         nextActionTime.reset();
      }
   }
   else if ( JT_OFFDELAY == nextState )
   {
      if ( isValidActionTime( actionParameter->offDelayTime ) )
      {
         SET_STATE_L1( JT_OFFDELAY );
         nextActionTime += convertToTime( actionParameter->offDelayTime );
      }
      else
      {
         jumpToNextState( JT_RAMP_OFF );
      }
   }
   else if ( JT_RAMP_OFF == nextState )
   {
      if ( isValidActionTime( actionParameter->rampOffTime ) )
      {
         SET_STATE_L1( JT_RAMP_OFF );
         calculateRampParameter();
      }
      else
      {
         jumpToNextState( JT_OFF );
      }
   }
   else if ( JT_OFF == nextState )
   {
      SET_STATE_L1( JT_OFF );
      if ( isValidActionTime( actionParameter->offTime ) )
      {
         nextActionTime += convertToTime( actionParameter->offTime );
      }
      else
      {
         nextActionTime.reset();
      }
   }
   else if ( JT_NO_JUMP_IGNORE_COMMAND == nextState )
   {
      nextActionTime.reset();
   }
   else
   {
      WARN_2( FSTR( "HmwDimmer::jumpToNextState() has no transition to state : " ), nextState );
      nextActionTime.reset();
   }
}

void HmwDimmer::handleStateChart( bool fromMainLoop )
{
   if ( actionParameter == NULL )
   {
      if ( state == START_UP )
      {
         setLevel( 0 );
         SET_STATE_L1( JT_OFF );
      }
      return;
   }

   switch ( state )
   {
      case JT_OFF:
      {
         setLevel( actionParameter->offLevel );
         jumpToNextState( actionParameter->jtOff );
         break;
      }
      case JT_ONDELAY:
      {
         if ( fromMainLoop )
         {
            jumpToNextState( actionParameter->jtOnDelay );
            break;
         }
      }
      case JT_RAMP_ON:
      {
         if ( !fromMainLoop || ( currentLevel >= actionParameter->onLevel ) || config->isDimmingModeSwitch() )
         {
            jumpToNextState( actionParameter->jtRampOn );
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
      case JT_ON:
      {
         setLevel( actionParameter->onLevel );
         jumpToNextState( actionParameter->jtOn );
         break;
      }
      case JT_OFFDELAY:
      {
         if ( fromMainLoop )
         {
            jumpToNextState( actionParameter->jtOffDelay );
            break;
         }
      }
      case JT_RAMP_OFF:
      {
         if ( !fromMainLoop || ( currentLevel <= actionParameter->offLevel ) || config->isDimmingModeSwitch() )
         {
            jumpToNextState( actionParameter->jtRampOff );
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

void HmwDimmer::calculateDimmingParameter()
{
   DEBUG_H1( FSTR( " calculateDimmingParameter" ) );

   uint8_t cutOffStart = Config::_00_PERCENT - config->getPwmRangeStart();
   uint8_t cutOffEnd = Config::_100_PERCENT - config->getPwmRangeEnd();

   dimmingOffset = normalizeLevel * cutOffStart * ( MAX_LEVEL / 10 );

   // use extra 16bit variable to have accurate calculation for the dimmingFactor
   uint16_t pwmRange = normalizeLevel * MAX_LEVEL;
   pwmRange = pwmRange - ( normalizeLevel * ( cutOffStart + cutOffEnd ) * ( MAX_LEVEL / 10 ) );

   dimmingFactor = pwmRange / MAX_LEVEL;

   DEBUG_M2( FSTR( "dimmingOffset : 0x" ), dimmingOffset );
   DEBUG_M2( FSTR( "dimmingFactor : 0x" ), dimmingFactor );
}

void HmwDimmer::calculateRampParameter()
{
   if ( ( state != JT_RAMP_ON ) && ( state != JT_RAMP_OFF ) )
   {
      ERROR_3( FSTR( "HmwDimmer::calculateRampParameter for state: 0x" ), (uint8_t)state, FSTR( " not implemented" ) );
      return;
   }
   DEBUG_H1( FSTR( " calculateRampParameter" ) );
   rampStep = actionParameter->onLevel - actionParameter->offLevel;
   nextStepTime = actionParameter->rampOnTime;

   if ( state == JT_RAMP_OFF )
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
