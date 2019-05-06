/*
 * HBWDS1820.cpp
 *
 *  Created on: 26.04.2017
 *      Author: Viktor Pankraz
 */

#include "HmwSHTC3.h"
#include "HmwDevice.h"

#include <Security/Crc8.h>
#include <Tracing/Logger.h>
#include <util/delay.h>
#include <stdlib.h>

#define getId() FSTR( "HmwSHTC3 " )

const uint8_t HmwSHTC3::debugLevel( DEBUG_LEVEL_OFF );

HmwSHTC3::HmwSHTC3( Twi& _hardware, Config* _config ) :
   hardware( &_hardware ),
   isSleeping( true ),
   currentHumidity( 0 ),
   lastSentHumidity( 0 ),
   currentCentiCelsius( 0 ),
   lastSentCentiCelsius( 0 )
{
   type = HmwChannel::HMW_SHTC3;
   config = _config;
   lastActionTime = 0;
   SET_STATE_L1( CHECK_SENSOR );
}


uint8_t HmwSHTC3::get( uint8_t* data )
{
   // MSB first
   *data++ = ( currentCentiCelsius >> 8 ) & 0xFF;
   *data++ = currentCentiCelsius & 0xFF;
   *data = currentHumidity;
   return sizeof( currentCentiCelsius ) + sizeof( currentHumidity );
}

void HmwSHTC3::loop( uint8_t channel )
{
   if ( !nextActionDelay || ( lastActionTime.since() < nextActionDelay ) )
   {
      return;
   }

   lastActionTime = Timestamp();   // at least last time of trying

   switch ( state )
   {
      case CHECK_SENSOR:
      {
         if ( checkSensorId() != OK )
         {
            // retry in 10s
            nextActionDelay = 10000;
            return;
         }
         SET_STATE_L1( START_MEASUREMENT );
      }
      case START_MEASUREMENT:
      {
         if ( startMeasurement() == OK )
         {
            // give some time for measuring before trying to read back the results
            nextActionDelay = 20;
            SET_STATE_L1( READ_MEASUREMENT );
         }
         else
         {
            // retry after 1s
            nextActionDelay = 1000;
         }
         break;
      }

      case READ_MEASUREMENT:
      {
         if ( readMeasurement() != OK )
         {
            // retry after 1s
            nextActionDelay = 1000;
            SET_STATE_L1( START_MEASUREMENT );
            return;
         }
         SET_STATE_L1( SEND_FEEDBACK );
      }

      case SEND_FEEDBACK:
      {
         bool doSend = true;

         // do not send before min interval
         doSend &= !( config->minInterval && ( lastSentTime.since() < ( (uint32_t)config->minInterval * 1000 ) ) );
         doSend &= ( ( config->maxInterval && ( lastSentTime.since() >= ( (uint32_t)config->maxInterval * 1000 ) ) )
                   || ( config->minHumidityDelta && ( abs( currentHumidity - lastSentHumidity ) >= config->minHumidityDelta ) )
                   || ( config->minTempDelta && ( abs( currentCentiCelsius - lastSentCentiCelsius ) >= ( (int16_t)config->minTempDelta * 10 ) ) ) );

         if ( doSend )
         {
            uint8_t data[3];
            uint8_t errcode = HmwDevice::sendInfoMessage( channel, get( data ), data );

            // sendInfoMessage returns 0 on success, 1 if bus busy, 2 if failed
            if ( errcode != 0 )
            {
               // retry in 500ms if something fails
               nextActionDelay = 500;
               break;
            }
            lastSentCentiCelsius = currentCentiCelsius;
            lastSentTime = Timestamp();
         }
         sleep(); // switch off sensor

         // start next measurement after 5s
         SET_STATE_L1( START_MEASUREMENT );
         nextActionDelay = 5000;
         break;
      }

      default:
      {

      }
   }
}

void HmwSHTC3::checkConfig()
{
   if ( config->minHumidityDelta > 100 )
   {
      config->minHumidityDelta = 2;
   }
   if ( config->minTempDelta > 250 )
   {
      config->minTempDelta = 5;
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

HmwSHTC3::HwStatus HmwSHTC3::checkSensorId()
{
   if ( wakeup() == OK )
   {
      if ( sendCommand( SHTC3_CMD_READ_ID ) == OK )
      {
         uint8_t data[DATA_SIZE_READ_ID];
         if ( hardware->master.read( TWI_ADDRESS, &data, sizeof( data ) ) == sizeof( data ) )
         {
            uint16_t myId = ( data[0] << 8 ) | data[1];
            if ( checkCrc( myId, data[2] ) == OK )
            {
               if ( ( myId & 0x083F ) == 0x0807 )
               {
                  // yes, this is the expected sensor
                  return OK;
               }
               ERROR_1( FSTR( "Sensor ID does not match" ) );
            }
            ERROR_1( FSTR( "CRC error" ) );
         }
         ERROR_1( FSTR( "TWI read error" ) );
      }
   }
   return FAILTURE;
}

HmwSHTC3::HwStatus HmwSHTC3::startMeasurement()
{
   if ( wakeup() == OK )
   {
      // start measurement with ClockStretching enabled, TemperaturFirst, NormalPowerMode
      if ( sendCommand( SHTC3_CMD_CSE_TF_NPM ) == OK )
      {
         return OK;
      }
   }
   return FAILTURE;
}

HmwSHTC3::HwStatus HmwSHTC3::readMeasurement()
{
   if ( wakeup() == OK )
   {
      uint8_t data[DATA_SIZE_READ_TH];
      if ( hardware->master.read( TWI_ADDRESS, &data, sizeof( data ) ) == sizeof( data ) )
      {
         uint16_t rawData = ( data[0] << 8 ) | data[1];
         if ( checkCrc( rawData, data[2] ) == OK )
         {
            currentCentiCelsius = convertToCentiCelsius( rawData );
            rawData = ( data[3] << 8 ) | data[4];
            if ( checkCrc( rawData, data[5] ) == OK )
            {
               currentHumidity = convertToRelativeHumidity( rawData );
               return OK;
            }
            ERROR_1( FSTR( "CRC error humidity" ) );
         }
         ERROR_1( FSTR( "CRC error temperature" ) );
      }
      ERROR_1( FSTR( "TWI read error" ) );
   }
   return FAILTURE;
}

int16_t HmwSHTC3::convertToCentiCelsius( uint16_t rawValue )
{
   return ( ( (int32_t)17500 * rawValue ) >> 16 ) - 4500;
}

uint8_t HmwSHTC3::convertToRelativeHumidity( uint16_t rawValue )
{
   return ( ( (uint32_t)100 * rawValue ) >> 16 );
}

HmwSHTC3::HwStatus HmwSHTC3::checkCrc( uint16_t _value, uint8_t _crc )
{
   uint8_t data[2] = { HBYTE( _value ), LBYTE( _value ) };
   uint8_t crc = 0xFF;
   uint8_t poly = 0x31;

   for ( uint8_t indi = 0; indi < 2; indi++ )
   {
      crc ^= data[indi];

      for ( uint8_t indj = 0; indj < 8; indj++ )
      {
         if ( crc & 0x80 )
         {
            crc = (uint8_t)( ( crc << 1 ) ^ poly );
         }
         else
         {
            crc <<= 1;
         }
      }
   }

   return ( _crc ^ crc ) ? FAILTURE : OK;
}

HmwSHTC3::HwStatus HmwSHTC3::sendCommand( HmwSHTC3::Commands cmd )
{
   if ( hardware->master.write( TWI_ADDRESS, &cmd, sizeof( cmd ) ) != sizeof( cmd ) )
   {
      ERROR_3( FSTR( "sendCommand(0x" ), (uint16_t)cmd, FSTR( " failed" ) );
      return FAILTURE;
   }
   return OK;
}

HmwSHTC3::HwStatus HmwSHTC3::softReset()
{
   HwStatus status = sendCommand( SHTC3_CMD_SFT_RST );
   _delay_us( 500 );
   return status;
}

HmwSHTC3::HwStatus HmwSHTC3::wakeup()
{
   HwStatus status = OK;
   if ( isSleeping )
   {
      status = sendCommand( SHTC3_CMD_WAKE );
      if ( status == OK )
      {
         isSleeping = false;
      }
      _delay_us( 250 );
   }
   return status;
}

HmwSHTC3::HwStatus HmwSHTC3::sleep()
{
   HwStatus status = OK;
   if ( !isSleeping )
   {
      status = sendCommand( SHTC3_CMD_SLEEP );
      if ( status == OK )
      {
         isSleeping = true;
      }
   }
   return status;
}