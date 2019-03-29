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

#define getId() FSTR( "HmwBrightness." ) << channel << ' '


const uint8_t HmwBrightness::debugLevel( DEBUG_LEVEL_LOW );

HmwBrightness::HmwBrightness( PortPin _portPin, TimerCounterChannel _tcChannel, Config* _config ) :
   measurePin( _portPin ),
   tcChannel( _tcChannel ),
   state( START_MEASUREMENT ),
   config( _config ),
   startCount( 0 ),
   currentValue( 0 ),
   lastSentValue( 0 )
{
   lastActionTime.reset();
   lastSentTime.reset();
   measurePin.set();
}

uint8_t HmwBrightness::get( uint8_t* data )
{
   // MSB first
   *data++ = ( currentValue >> 8 ) & 0xFF;
   *data = currentValue & 0xFF;
   return 2;
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
      tcChannel.clearCCFlag();
      startCount = tcChannel.getCurrentCount();
      tcChannel.enable();
      measurePin.configInput();
      nextActionDelay = 10;
      lastActionTime = Timestamp();
      state = WAIT_MEASUREMENT_RESULT;
   }
   else if ( state == WAIT_MEASUREMENT_RESULT )
   {
      nextActionDelay += 10;
      if ( tcChannel.getCCFlag() )
      {
         uint16_t delta = ( tcChannel.getCapture() - startCount ) >> 3;
         if ( delta > MAX_VALUE )
         {
            currentValue = 0;
         }
         else
         {
            currentValue = MAX_VALUE - delta;
         }
         DEBUG_H3( nextActionDelay, '\t', delta );
         DEBUG_L2( ' ', currentValue );
         tcChannel.disable();
         measurePin.configOutput();
         state = SEND_FEEDBACK;
      }
   }

   if ( nextActionDelay > MAX_MEASURE_TIME )
   {
      // this is handled as a timeout, it is too dark for measurement
      tcChannel.disable();
      measurePin.configOutput();
      state = START_MEASUREMENT;
   }

   if ( state == SEND_FEEDBACK )
   {
      // do not send before min interval
      bool doSend = true;
      doSend &= !( config->minInterval && ( lastSentTime.since() < ( (uint32_t)config->minInterval * 1000 ) ) );
      doSend &= ( ( config->maxInterval && ( lastSentTime.since() >= ( (uint32_t)config->maxInterval * 1000 ) ) )
                || ( config->minDelta && ( (uint16_t)abs( currentValue - lastSentValue ) >= ( (uint16_t)config->minDelta ) ) ) );

      if ( doSend )
      {
         uint8_t data[2];
         if ( HmwDevice::sendInfoMessage( channel, get( data ), data ) == IStream::SUCCESS )
         {
            lastSentValue = currentValue;
            lastSentTime = Timestamp();
         }
      }
      // start next measurement only once a second, if last measurement took longer
      // the next one will start after 10ms
      if ( nextActionDelay < MIN_MEASURE_DELAY )
      {
         nextActionDelay = MIN_MEASURE_DELAY;
      }
      state = START_MEASUREMENT;
   }
}

void HmwBrightness::checkConfig()
{
   if ( config->minDelta > 250 )
   {
      config->minDelta = 5;
   }
   if ( config->minInterval && ( ( config->minInterval < 5 ) || ( config->minInterval > 3600 ) ) )
   {
      config->minInterval = 10;
   }
   if ( config->maxInterval && ( ( config->maxInterval < 5 ) || ( config->maxInterval > 3600 ) ) )
   {
      config->maxInterval = 150;
   }
   if ( config->maxInterval && ( config->maxInterval < config->minInterval ) )
   {
      config->maxInterval = 0;
   }
}