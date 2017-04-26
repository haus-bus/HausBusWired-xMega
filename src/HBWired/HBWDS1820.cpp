/*
 * HBWDS1820.cpp
 *
 *  Created on: 26.04.2017
 *      Author: Viktor Pankraz
 */

#include "HBWDS1820.h"
#include <Security/Crc8.h>

bool HBWDS1820::selfPowered( true );


HBWDS1820::HBWDS1820( const OneWire& _hardware, Config* _config  ) :
    hardware( _hardware ), state(START_MEASUREMENT)
{
    config = _config;
    nextActionDelay = 2000;
    lastActionTime = 0;
}

bool HBWDS1820::isSelfPowered()
{
  hardware.reset();
  hardware.sendCommand( READ_POWER_SUPPLY, (uint8_t*) &romCode );

  selfPowered = (hardware.sendReceiveBit( 1 ) ? true : false);

  hardware.reset();
  return selfPowered;
}

bool HBWDS1820::isSensor( uint8_t familiyCode )
{
  return ((familiyCode == DS18B20_ID) || (familiyCode == DS18S20_ID));
}


uint8_t HBWDS1820::get(uint8_t* data)
{
  uint8_t sp[SCRATCHPAD_SIZE];

  hardware.reset();
  hardware.sendCommand( READ, (uint8_t*) &romCode );

  for ( uint8_t i = 0; i < SCRATCHPAD_SIZE; i++ )
    sp[i] = hardware.read();
  if ( Crc8::hasError( sp, SCRATCHPAD_SIZE ) )
  {
    return 0; // CRC_FAILTURE;
  }
  uint16_t centiCelsius = convertToCentiCelsius( sp );

  // LSB first
  *data++ = centiCelsius & 0xFF;
  *data   = (centiCelsius>>8) & 0xFF;
  return 2;
}

void HBWDS1820::loop(HBWDevice* device, uint8_t channel)
{
    if( !nextActionDelay )
    {
        return;
    }

    unsigned long now = millis();
    if( ( now - lastActionTime ) < nextActionDelay )
    {
        return;
    }

    lastActionTime = now;  // at least last time of trying

    if( state == SEARCH_SENSOR )
    {
        // check for 1-Wire components
        uint8_t diff = OneWire::SEARCH_FIRST;

        while ( diff != OneWire::LAST_DEVICE )
        {
            diff = hardware.searchROM( diff, (uint8_t*) &romCode );
            if ( diff == OneWire::PRESENCE_ERROR )
            {
                hbwdebug(F("C: No devices found\n"));
                break;
            }
            else if ( diff == OneWire::DATA_ERROR )
            {
                hbwdebug( F("C: 1-Wire Bus Error") );
                break;
            }
            else
            {
                hbwdebug( F("I: 0x") );
                for ( uint8_t i = 0; i < OneWire::ROMCODE_SIZE; i++ )
                {
                    hbwdebug( ((uint8_t* )&romCode)[i] );
                }

                if ( isSensor( romCode.family ) )
                {
                    hbwdebug( F("->DS18X20\n") );
                    state = START_MEASUREMENT;
                    nextActionDelay = 100;
                    return;
                }
                else
                {
                    hbwdebug( F("->UNKNOWN\n") );
                }
            }
        }
        // no sensor found retry search later
        nextActionDelay = 1000;

    }
    else if ( state == START_MEASUREMENT )
    {
        if( startMeasurement() == OK )
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
        uint8_t data[2];
        uint8_t errcode = device->sendInfoMessage(channel, get(data), data);

        // sendInfoMessage returns 0 on success, 1 if bus busy, 2 if failed
        if( errcode )
        {
            // bus busy
            // try again later, but insert a small delay
            nextActionDelay = 250;
        }
        else
        {
            // start next measurement after 10s
            state = START_MEASUREMENT;
            nextActionDelay = 10000;
        }
    }
}


HBWDS1820::HwStatus HBWDS1820::startMeasurement( bool allSensors )
{
  hardware.reset();

  if ( hardware.isIdle() )
  { // only send if bus is "idle" = high
    hardware.sendCommand( CONVERT_T, allSensors ? 0 : (uint8_t*) &romCode );
    if ( !selfPowered )
    {
      hardware.enableParasite();
    }
    return OK;
  }
  else
  {
    // ERROR_1( "OW not idle on startMeasurement" );
    return START_FAIL;
  }
}

uint16_t HBWDS1820::convertToCentiCelsius( uint8_t* scratchPad )
{
  uint16_t measurement = scratchPad[0];         // LSB
  measurement |= ((uint16_t) scratchPad[1]) << 8;  // MSB

  //  only work on 12bit-base
  if ( romCode.family == DS18S20_ID ) // 9 -> 12 bit if 18S20
  {
    // Extended measurements for DS18S20
    measurement &= (uint16_t) 0xfffe;	// Discard LSB , needed for later extended precicion calc
    measurement <<= 3;// Convert to 12-bit , now degrees are in 1/16 degrees units
    measurement += (16 - scratchPad[6]) - 4;// Add the compensation , and remember to subtract 0.25 degree (4/16)
  }

  // clear undefined bits for B != 12bit
  if ( romCode.family == DS18B20_ID ) // check resolution 18B20
  {
    uint8_t i = scratchPad[DS18B20_CONF_REG];
    if ( (i & DS18B20_12_BIT) == DS18B20_12_BIT ){}
    else if ( (i & DS18B20_11_BIT) == DS18B20_11_BIT )
      measurement &= ~(DS18B20_11_BIT_UNDF);
    else if ( (i & DS18B20_10_BIT) == DS18B20_10_BIT )
      measurement &= ~(DS18B20_10_BIT_UNDF);
    else
    { // if ( (i & DS18B20_9_BIT) == DS18B20_9_BIT ) {
      measurement &= ~(DS18B20_9_BIT_UNDF);
    }
  }

  uint16_t centiCelsius = static_cast<int8_t>( measurement >> 4 ) * 100;
  uint8_t fracture = (uint8_t) (measurement & 0x000F);
  if ( fracture & 0x8 ) centiCelsius += 50;
  if ( fracture & 0x4 ) centiCelsius += 25;
  if ( fracture & 0x2 ) centiCelsius += 13;
  if ( fracture & 0x1 ) centiCelsius += 6;

  return centiCelsius;
}
