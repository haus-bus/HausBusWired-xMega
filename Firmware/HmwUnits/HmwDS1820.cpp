/*
 * HBWDS1820.cpp
 *
 *  Created on: 26.04.2017
 *      Author: Viktor Pankraz
 */

#include "HmwDS1820.h"
#include "HmwDevice.h"

#include <Security/Crc8.h>
#include <Tracing/Logger.h>
#include <stdlib.h>

#define getId() FSTR( "HmwDS1820." ) << channel

#define INVALID_VALUE -27315

const uint8_t HmwDS1820::debugLevel( DEBUG_LEVEL_OFF );

bool HmwDS1820::selfPowered( true );

HmwDS1820::HmwDS1820( OneWire& _hardware, Config* _config ) :
   hardware( &_hardware ), state( SEARCH_SENSOR )
{
   type = HmwChannel::HMW_DS18X20;
   config = _config;
   lastActionTime = 0;
}

bool HmwDS1820::isSelfPowered()
{
   hardware->reset();
   hardware->sendCommand( READ_POWER_SUPPLY, (uint8_t*) &romCode );

   selfPowered = ( hardware->sendReceiveBit( 1 ) ? true : false );

   hardware->reset();
   return selfPowered;
}

bool HmwDS1820::isSensor( uint8_t familiyCode )
{
   return ( ( familiyCode == DS18B20_ID ) || ( familiyCode == DS18S20_ID ) );
}


uint8_t HmwDS1820::get( uint8_t* data )
{
   // MSB first
   *data++ = ( currentCentiCelsius >> 8 ) & 0xFF;
   *data = currentCentiCelsius & 0xFF;
   return 2;
}

void HmwDS1820::loop( uint8_t channel )
{
   if ( !nextActionDelay || ( ( config->id == 0 ) && ( state != SEND_INVALID_VALUE ) ) )
   {
      return;
   }

   if ( lastActionTime.since() < nextActionDelay )
   {
      return;
   }

   lastActionTime = Timestamp();   // at least last time of trying

   if ( state == SEARCH_SENSOR )
   {
      // set the last measured value to an invalid one
      currentCentiCelsius = INVALID_VALUE;

      // search for a special sensor
      uint8_t diff = OneWire::SEARCH_FIRST;
      DEBUG_H2( FSTR( " searching for 0x" ), config->id );

      // check for 1-Wire components
      while ( diff != OneWire::LAST_DEVICE )
      {
         uint8_t currentId = 0;
         diff = hardware->searchROM( diff, (uint8_t*) &romCode );
         if ( diff == OneWire::PRESENCE_ERROR )
         {
            DEBUG_H1( FSTR( " No devices found" ) );
            break;
         }
         else if ( diff == OneWire::DATA_ERROR )
         {
            ERROR_2( getId(), FSTR( " Bus Error" ) );
            break;
         }
         else
         {
            DEBUG_H1( FSTR( " 0x" ) );
            for ( uint8_t i = 0; i < OneWire::ROMCODE_SIZE; i++ )
            {
               DEBUG_L1( ( (uint8_t* )&romCode )[i] );
               currentId += ( (uint8_t* )&romCode )[i];
            }

            if ( isSensor( romCode.family ) )
            {
               DEBUG_L2( FSTR( "->DS18X20, ID:0x" ), currentId );
               if ( ( config->id == 0xFF ) && !isUsed( currentId ) )
               {
                  config->id = currentId;
               }
               if ( config->id == currentId )
               {
                  state = START_MEASUREMENT;
                  nextActionDelay = 100;
                  return;
               }

            }
            else
            {
               DEBUG_L1( FSTR( "->UNKNOWN" ) );
            }
         }
      }
      // no sensor found, stop channel
      DEBUG_H1( FSTR( " No sensor for this channel" ) );
      nextActionDelay = 5000;
      state = SEND_INVALID_VALUE;
   }
   else if ( state == START_MEASUREMENT )
   {
      if ( startMeasurement() == OK )
      {
         nextActionDelay = 1000;
         state = SEND_FEEDBACK;
      }
      else
      {
         // retry after 250ms
         nextActionDelay = 250;
      }
   }
   else if ( state == SEND_FEEDBACK )
   {
      bool doSend = ( readMeasurement() == OK );

      // do not send before min interval
      doSend &= !( config->minInterval && ( lastSentTime.since() < ( (uint32_t)config->minInterval * 1000 ) ) );
      doSend &= ( ( config->maxInterval && ( lastSentTime.since() >= ( (uint32_t)config->maxInterval * 1000 ) ) )
                || ( config->minDelta && ( (uint16_t)abs( currentCentiCelsius - lastSentCentiCelsius ) >= ( (uint16_t)config->minDelta * 10 ) ) ) );

      if ( doSend )
      {
         uint8_t data[2];
         uint8_t errcode = HmwDevice::sendInfoMessage( channel, get( data ), data );

         // sendInfoMessage returns 0 on success, 1 if bus busy, 2 if failed
         if ( errcode == 0 )
         {
            lastSentCentiCelsius = currentCentiCelsius;
            lastSentTime = Timestamp();
         }
      }
      // start next measurement after 5s
      state = START_MEASUREMENT;
      nextActionDelay = 5000;
   }
   else if ( state == SEND_INVALID_VALUE )
   {
      // send the INVALID_VALUE one time
      uint8_t data[2];
      HmwDevice::sendInfoMessage( channel, get( data ), data );
      nextActionDelay = 0;
   }

}

void HmwDS1820::checkConfig()
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

   // maybe someone has changed the Ids, search the desired sensor now
   state = SEARCH_SENSOR;
   nextActionDelay = 500;
}


HmwDS1820::HwStatus HmwDS1820::startMeasurement( bool allSensors )
{
   hardware->reset();

   if ( hardware->isIdle() )
   { // only send if bus is "idle" = high
      hardware->sendCommand( CONVERT_T, allSensors ? 0 : (uint8_t*) &romCode );
      if ( !selfPowered )
      {
         hardware->enableParasite();
      }
      return OK;
   }
   else
   {
      // ERROR_1( "OW not idle on startMeasurement" );
      return START_FAIL;
   }
}

HmwDS1820::HwStatus HmwDS1820::readMeasurement()
{

   uint8_t sp[SCRATCHPAD_SIZE];

   hardware->reset();
   hardware->sendCommand( READ, (uint8_t*) &romCode );

   for ( uint8_t i = 0; i < SCRATCHPAD_SIZE; i++ )
   {
      sp[i] = hardware->read();
   }
   if ( Crc8::hasError( sp, SCRATCHPAD_SIZE ) )
   {
      return CRC_FAILTURE;
   }
   currentCentiCelsius = convertToCentiCelsius( sp );

   return OK;
}

int16_t HmwDS1820::convertToCentiCelsius( uint8_t* scratchPad )
{
   uint16_t measurement = scratchPad[0];        // LSB
   measurement |= ( (uint16_t) scratchPad[1] ) << 8; // MSB

   // only work on 12bit-base
   if ( romCode.family == DS18S20_ID ) // 9 -> 12 bit if 18S20
   {
      // Extended measurements for DS18S20
      measurement &= (uint16_t) 0xfffe; // Discard LSB , needed for later extended precicion calc
      measurement <<= 3;// Convert to 12-bit , now degrees are in 1/16 degrees units
      measurement += ( 16 - scratchPad[6] ) - 4;// Add the compensation , and remember to subtract 0.25 degree (4/16)
   }

   // clear undefined bits for B != 12bit
   if ( romCode.family == DS18B20_ID ) // check resolution 18B20
   {
      uint8_t i = scratchPad[DS18B20_CONF_REG];
      if ( ( i & DS18B20_12_BIT ) == DS18B20_12_BIT )
      {
      }
      else if ( ( i & DS18B20_11_BIT ) == DS18B20_11_BIT )
      {
         measurement &= ~( DS18B20_11_BIT_UNDF );
      }
      else if ( ( i & DS18B20_10_BIT ) == DS18B20_10_BIT )
      {
         measurement &= ~( DS18B20_10_BIT_UNDF );
      }
      else
      { // if ( (i & DS18B20_9_BIT) == DS18B20_9_BIT ) {
         measurement &= ~( DS18B20_9_BIT_UNDF );
      }
   }

   int16_t centiCelsius = static_cast<int8_t>( measurement >> 4 ) * 100;
   uint8_t fracture = (uint8_t) ( measurement & 0x000F );
   if ( fracture & 0x8 )
   {
      centiCelsius += 50;
   }
   if ( fracture & 0x4 )
   {
      centiCelsius += 25;
   }
   if ( fracture & 0x2 )
   {
      centiCelsius += 13;
   }
   if ( fracture & 0x1 )
   {
      centiCelsius += 6;
   }

   return centiCelsius;
}


bool HmwDS1820::isUsed( uint8_t id )
{
   for ( uint8_t i = 0; i < HmwChannel::getNumChannels(); i++ )
   {
      HmwChannel* channel = HmwChannel::getChannel( i );
      if ( channel->isOfType( HmwChannel::HMW_DS18X20 ) )
      {
         HmwDS1820* sensor = reinterpret_cast<HmwDS1820*>( channel );
         if ( sensor->getConfigId() == id )
         {
            return true;
         }
      }
   }
   return false;
}