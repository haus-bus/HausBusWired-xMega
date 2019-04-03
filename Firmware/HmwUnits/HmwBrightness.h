/*
 * HmwBrightness.h
 *
 * Created: 17.07.2018 21:04:23
 * Author: Viktor Pankraz
 */


#ifndef __HMWBRIGHTNESS_H__
#define __HMWBRIGHTNESS_H__

#include <Time/Timestamp.h>
#include <xEeprom.h>
#include <DigitalOutput.h>
#include <Peripherals/TimerCounterChannel.h>

#include "HmwChannel.h"


class HmwBrightness : public HmwChannel
{
   public:
      struct Config
      {
         uint8_tx options;
         uint8_tx minDelta;
         uint16_tx minInterval;
         uint16_tx maxInterval;
      };

      enum HwStatus
      {
         OK,
         FAILTURE,
         NO_RESPONSE
      };

      enum State
      {
         START_MEASUREMENT,
         WAIT_MEASUREMENT_RESULT,
         SEND_FEEDBACK,
         SEND_INVALID_VALUE
      };

      static const uint16_t MAX_MEASURE_TIME = 2000;
      static const uint16_t MAX_VALUE = 4096;
      static const uint16_t MIN_MEASURE_DELAY = 1000;

      ////    Constructor and destructor    ////
      HmwBrightness( PortPin _portPin, TimerCounterChannel _tcChannel, Config* _config );

      // definition of needed functions from HBWChannel class
      virtual uint8_t get( uint8_t* data );
      virtual void loop( uint8_t channel );
      virtual void checkConfig();

      inline bool isDisabled()
      {
         return !config->minDelta && !config->minInterval && !config->maxInterval;
      }

   protected:

   private:

      ////    Additional operations    ////

   public:

   protected:
   private:

      ////    Attributes    ////

   public:

   private:

      static const uint8_t debugLevel;

      DigitalOutput measurePin;

      TimerCounterChannel tcChannel;

      State state;

      Config* config;

      uint16_t startCount;

      uint16_t currentValue;

      uint16_t lastSentValue;

      Timestamp lastActionTime;

      Timestamp lastSentTime;


}; // HmwBrightness

#endif // __HMWBRIGHTNESS_H__
