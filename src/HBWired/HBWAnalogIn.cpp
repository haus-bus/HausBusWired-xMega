/*
 * HBWAnalogIn.cpp
 *
 *  Created on: 26.04.2017
 *      Author: Viktor Pankraz
 */

#include "HBWAnalogIn.h"

HBWAnalogIn::HBWAnalogIn( ADC_t* _adc, uint8_t _adcChannel, Config* _config ) :
adc( _adc ), adcChannel( _adcChannel ), config(_config), state(START_MEASUREMENT)
{
    nextActionDelay = 10;
    lastActionTime = 0;
}


uint8_t HBWAnalogIn::get(uint8_t* data)
{
  // MSB first
  *data++ = (currentValue>>8) & 0xFF;
  *data   = currentValue & 0xFF;
  return 2;
}

void HBWAnalogIn::loop(HBWDevice* device, uint8_t channel)
{
    if( !nextActionDelay )
    {
        return;
    }

    if( lastActionTime.since() < nextActionDelay )
    {
        return;
    }

    lastActionTime = Timestamp();  // at least last time of trying

    if( state == START_MEASUREMENT )
    {
        adc_enable(adc);
        adc_start_conversion(adc, adcChannel);
        nextActionDelay = 1;
    }
    else if ( state == SAMPLE_VALUES )
    {
        static uint16_t buffer[8] = { 0,0,0,0,0,0,0,0 };
        static uint8_t nextIndex = 0;

        if( adc_get_interrupt_flag(adc, adcChannel) )
        {
            adc_clear_interrupt_flag( adc, adcChannel );
            buffer[nextIndex++] = adc_get_unsigned_result( adc, adcChannel );

            if( nextIndex >= sizeof(buffer) )
            {
                uint32_t sum = 0;
                uint8_t i=sizeof(buffer); 
                do
                {
                    sum += buffer[--i];
                } while(i);
                currentValue = sum / sizeof(buffer);
                state = SEND_FEEDBACK;
            }
        }
    }
    else if ( state == SEND_FEEDBACK )
    {
        bool doSend = true;

        // do not send before min interval
        doSend &= !( config->minInterval && ( lastSentTime.since() < ( (uint32_t)config->minInterval * 1000) ) );
        doSend &= (( config->maxInterval && ( lastSentTime.since() >= ( (uint32_t)config->maxInterval * 1000) ) ) 
                || ( config->minDelta && ( (uint16_t)abs(currentValue - lastSentValue)  >= ( (uint16_t)config->minDelta * 10) ) ) );

        if( doSend )
        {
            uint8_t data[2];
            uint8_t errcode = device->sendInfoMessage(channel, get(data), data);

            // sendInfoMessage returns 0 on success, 1 if bus busy, 2 if failed
            if( errcode )
            {
                // bus busy
                // try again later, but insert a small delay
                nextActionDelay = 250;
                return;
            }
            lastSentValue = currentValue;
            lastSentTime = Timestamp();

        }
        // start next measurement after 1s
        state = START_MEASUREMENT;
        nextActionDelay = 1000;

    }
}
