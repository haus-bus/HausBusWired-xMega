/*
 * HmwSHTC3.h
 *
 *  Created on: 26.04.2017
 *      Author: Viktor Pankraz
 */

#ifndef HmwSHTC3_H
#define HmwSHTC3_H

#include <Peripherals/Twi.h>
#include <Time/Timestamp.h>
#include <xEeprom.h>

#include "HmwChannel.h"


class HmwSHTC3 : public HmwChannel
{
   public:

      static const uint8_t TWI_ADDRESS = 0x70;
      static const uint8_t DATA_SIZE_READ_ID = 0x03;
      static const uint8_t DATA_SIZE_READ_TH = 0x06;

      enum HwStatus
      {
         OK,
         START_FAIL,
         FAILTURE,
         CRC_FAILTURE,
         OUT_OF_MEMORY
      };

      enum States
      {
         CHECK_SENSOR,
         START_SENSOR,
         START_MEASUREMENT,
         READ_MEASUREMENT,
         STOP_SENSOR,
         SEND_FEEDBACK,
         SEND_INVALID_VALUE
      };

      // Because MSB has to be sent before LSB, the bytes in the following enum are swapped. TWI sends LSB first
      enum Commands
      {
         SHTC3_CMD_WAKE = 0x1735,
         SHTC3_CMD_SLEEP = 0x98B0,
         SHTC3_CMD_SFT_RST = 0x5D80,
         SHTC3_CMD_READ_ID = 0xC8EF,

         SHTC3_CMD_CSE_RHF_NPM = 0x245C,                // Clock stretching, RH first, Normal power mode
         SHTC3_CMD_CSE_RHF_LPM = 0xDE44,                // Clock stretching, RH first, Low power mode
         SHTC3_CMD_CSE_TF_NPM = 0xA27C,         // Clock stretching, T first, Normal power mode
         SHTC3_CMD_CSE_TF_LPM = 0x5864,         // Clock stretching, T first, Low power mode

         SHTC3_CMD_CSD_RHF_NPM = 0xE058,                // Polling, RH first, Normal power mode
         SHTC3_CMD_CSD_RHF_LPM = 0x1A40,                // Polling, RH first, Low power mode
         SHTC3_CMD_CSD_TF_NPM = 0x6678,         // Polling, T first, Normal power mode
         SHTC3_CMD_CSD_TF_LPM = 0x9C60             // Polling, T first, Low power mode
      };

      struct Config
      {
         uint16_tx minInterval;
         uint8_tx minDeltaPercent;
         uint8_tx reserve1;
         uint16_tx reserve2;
      };

      ////    Constructors and destructors    ////

      HmwSHTC3( Twi& _hardware, Config* _config );

      ////    Operations    ////

      HwStatus softReset();

      HwStatus checkSensorId();

      HwStatus readMeasurement();

      HwStatus startMeasurement();

      // definition of needed functions from HBWChannel class
      virtual uint8_t get( uint8_t* data );
      virtual void loop( uint8_t channel );
      virtual void checkConfig();

   private:

      inline void setMainState( States _state )
      {
         state = _state;
      }

      HwStatus sendCommand( Commands cmd );

      HwStatus sleep();

      HwStatus wakeup();

      HwStatus checkCrc( uint16_t value, uint8_t crc );

      int16_t convertToCentiCelsius( uint16_t rawValue );

      uint8_t convertToRelativeHumidity( uint16_t rawValue );

      ////    Additional operations    ////

   public:


   private:


      ////    Attributes    ////

   public:

      Twi* hardware;

   private:

      static const uint8_t debugLevel;

      bool isSleeping;

      States state;

      uint8_t currentHumidity;

      uint8_t lastSentHumidity;

      uint16_t nextActionDelay;

      int16_t currentCentiCelsius;

      int16_t lastSentCentiCelsius;

      Timestamp lastActionTime;

      Config* config;
};


#endif

