/* 
* HBWDimmer.cpp
*
* Created: 26.04.2017 09:01:56
* Author: viktor.pankraz
*/


#include "HBWDimmer.h"

HBWDimmer::HBWDimmer( PwmOutput* _pwmOutput, Config* _config )
{
    pwmOutput = _pwmOutput;
    config = _config;
    nextFeedbackDelay = 0;
    lastFeedbackTime = 0;

} //HBWDimmer


void HBWDimmer::set(HBWDevice* device, uint8_t length, uint8_t const * const data) {
    
    if ( *data > 200 ) // toggle
    {   
        if( pwmOutput->getDutyCycle() ) 
        {
            pwmOutput->clear();
        }
        else
        {
            pwmOutput->setDutyCycle();
        }
    }
    else
    {
        // the default range is 0-200, this must be mapped to 0-100% duty cycle
        pwmOutput->setDutyCycle( *data/2 );
    }

    // Logging
    if( !nextFeedbackDelay && config->logging ) 
    {
        lastFeedbackTime = millis();
        nextFeedbackDelay = device->getLoggingTime() * 100;
    }
};


uint8_t HBWDimmer::get(uint8_t* data) 
{
    // map 0-100% to 0-200
    (*data) = pwmOutput->getDutyCycle()<<1;
    return 1;
};

void HBWDimmer::loop(HBWDevice* device, uint8_t channel) 
{
    // feedback trigger set?
    if( !nextFeedbackDelay ) 
    {
        return;
    }

    unsigned long now = millis();
    if( ( now - lastFeedbackTime ) < nextFeedbackDelay ) 
    {
        return;
    }

    lastFeedbackTime = now;  // at least last time of trying

    
    uint8_t level;
    uint8_t errcode = device->sendInfoMessage(channel, get(&level), &level);

    // sendInfoMessage returns 0 on success, 1 if bus busy, 2 if failed
    if( errcode ) 
    {  
        // bus busy
        // try again later, but insert a small delay
        nextFeedbackDelay = 250;
    }
    else
    {
        nextFeedbackDelay = 0;
    }
}