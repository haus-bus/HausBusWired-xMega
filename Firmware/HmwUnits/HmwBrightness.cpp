/*
 * HmwBrightness.cpp
 *
 * Created: 17.07.2018 21:04:23
 * Author: Viktor Pankraz
 */


#include "HmwBrightness.h"
#include "HmwDevice.h"

#include <Tracing/Logger.h>
#include <stdlib.h>

const HmwBrightness::MeasurementRangeParameter HmwBrightness::rangeParams[HmwBrightness::MAX_RANGE_PARAMETER] =
{
   {
      TC_CLKSEL_DIV4_gc,
      1,
      2
   },
   {
      TC_CLKSEL_DIV64_gc,
      16,
      30
   },
   {
      TC_CLKSEL_DIV1024_gc,
      256,
      500
   }
};

#define getId() FSTR( "HmwBrightness." ) << channel << ' '


const uint8_t HmwBrightness::debugLevel( DEBUG_LEVEL_LOW );

HmwBrightness::HmwBrightness( PortPin _portPin, TimerCounterChannel _tcChannel, Config* _config ) :
   measurePin( _portPin ),
   tcChannel( _tcChannel ),
   state( START_MEASUREMENT ),
   rangeParamsSet( _8MHZ ),
   config( _config ),
   startCount( 0 ),
   currentValue( 0 ),
   lastSentValue( 0 ),
   filterHelper( 0 )
{
   lastActionTime.reset();
   measurePin.set();
   prepareNextMeasurement();
}


uint8_t HmwBrightness::get( uint8_t* data )
{
   // MSB first
   *data++ = HBYTE( currentValue );
   *data++ = LBYTE( currentValue );

   return 2;
}

void HmwBrightness::prepareNextMeasurement()
{
   tcChannel.disable();
   measurePin.configOutput();
   state = START_MEASUREMENT;

   // give some time to load capacitor for next measurement
   nextActionDelay = MIN_MEASURE_DELAY;
   lastActionTime = Timestamp();
}

void HmwBrightness::loop( uint8_t channel )
{
   if ( isDisabled() || ( lastActionTime.since() < nextActionDelay ) )
   {
      return;
   }

   if ( state == START_MEASUREMENT )
   {
      CriticalSection doNotInterrupt;
      tcChannel.getTimerCounter()->configClockSource( rangeParams[rangeParamsSet].clkSelection );
      tcChannel.clearCCFlag();
      startCount = tcChannel.getCurrentCount();
      tcChannel.enable();
      measurePin.configInput();
      nextActionDelay = 1;
      lastActionTime = Timestamp();
      state = WAIT_MEASUREMENT_RESULT;
   }
   else if ( state == WAIT_MEASUREMENT_RESULT )
   {
      if ( lastActionTime.since() > rangeParams[rangeParamsSet].measurementTimeout )
      {
         // this is handled as a timeout, switch range if possible and start new measurement
         if ( rangeParamsSet < ( MAX_RANGE_PARAMETER - 1 ) )
         {
            rangeParamsSet++;
         }
         prepareNextMeasurement();
      }
      else
      {
         // check for measurement result each ms
         nextActionDelay += 1;
         if ( tcChannel.getCCFlag() )
         {
            uint16_t delta = tcChannel.getCapture() - startCount;

            if ( ( delta < SWITCH_RANGE_THRESHOLD ) && ( rangeParamsSet > 0 ) )
            {
               // switch into higher resolution and measure again
               rangeParamsSet--;
               prepareNextMeasurement();
               return;
            }

            // make sure that result will be uint32_t (depends on type of first operand)
            uint16_t lux = log( (uint32_t)delta * rangeParams[rangeParamsSet].eighthMicroSecondPerTick ) * config->scale;
            uint16_t maxLux = config->offset * 10;

            lux = ( maxLux < lux ) ? 0 : maxLux - lux;

            // check if filter has to be initialized with first value
            if ( filterHelper == 0 )
            {
               filterHelper = ( lux << FILTER_LEVEL );
            }
            else
            {
               // filter new input values
               filterHelper += lux - currentValue;
            }
            currentValue = ( filterHelper >> FILTER_LEVEL );

            DEBUG_H3( lux, '\t', currentValue );
            tcChannel.disable();
            measurePin.configOutput();
            state = SEND_FEEDBACK;
         }
      }
   }

   if ( state == SEND_FEEDBACK )
   {
      bool doSend = perCentOf<uint32_t>( config->minDeltaPercent, lastSentValue ) <= (uint32_t)labs( currentValue - lastSentValue );

      if ( doSend && handleFeedback( SystemTime::S* config->minInterval ) )
      {
         lastSentValue = currentValue;
      }
      prepareNextMeasurement();
   }
}

void HmwBrightness::checkConfig()
{
   if ( config->minInterval && ( ( config->minInterval < 10 ) || ( config->minInterval > 3600 ) ) )
   {
      config->minInterval = 10;
   }
   if ( config->minDeltaPercent && ( ( config->minDeltaPercent < 10 ) || ( config->minDeltaPercent > 100 ) ) )
   {
      config->minDeltaPercent = 10;
   }
   if ( ( config->scale < 5 ) || ( config->scale > 200 ) )
   {
      config->scale = 60;
   }
   if ( ( config->offset < 5 ) || ( config->offset > 2500 ) )
   {
      config->offset = 52;
   }

   nextFeedbackTime = SystemTime::now() + SystemTime::S* config->minInterval;
}