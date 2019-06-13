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
         uint16_tx minInterval;
         uint8_tx minDeltaPercent;
         uint8_tx scale;
         uint16_tx offset;
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

      enum MeasurementRangeParameterSet
      {
         _8MHZ,
         _500KHZ,
         _31250HZ,
         MAX_RANGE_PARAMETER
      };

      struct MeasurementRangeParameter
      {
         TC_CLKSEL_t clkSelection;
         uint16_t eighthMicroSecondPerTick;
         uint16_t measurementTimeout;
      };

      static const uint16_t SWITCH_RANGE_THRESHOLD = 512;
      static const uint16_t MIN_MEASURE_DELAY = 500;
      static const uint8_t FILTER_LEVEL = 4;   // 0: no filtering -> 8: max filtering

      static const MeasurementRangeParameter rangeParams[MAX_RANGE_PARAMETER];

      ////    Constructor and destructor    ////
      HmwBrightness( PortPin _portPin, TimerCounterChannel _tcChannel, Config* _config );

      // definition of needed functions from HBWChannel class
      virtual uint8_t get( uint8_t* data );
      virtual void loop( uint8_t channel );
      virtual void checkConfig();

      inline bool isDisabled()
      {
         return !config->minDeltaPercent && !config->minInterval;
      }

   protected:

   private:

      ////    Additional operations    ////

   public:

   protected:
   private:

      void prepareNextMeasurement();

      ////    Attributes    ////

   public:

   private:

      static const uint8_t debugLevel;

      DigitalOutput measurePin;

      TimerCounterChannel tcChannel;

      State state;

      uint8_t rangeParamsSet;

      Config* config;

      uint16_t startCount;

      uint32_t currentValue;

      uint32_t lastSentValue;

      uint32_t filterHelper;

      Timestamp lastActionTime;

}; // HmwBrightness

#endif // __HMWBRIGHTNESS_H__
