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
    feedbackCmdActive = false;
    logicalState = OFF;
    currentLevel = 0;
    onLevel = 200;
    offLevel = 0;
    blinkOnTime = 10;
    blinkOffTime = 10;
    blinkQuantity = 255;
    nextFeedbackTime.reset();
    nextBlinkTime.reset();

} //HBWDimmer


void HBWDimmer::set(HBWDevice* device, uint8_t length, uint8_t const * const data) {
    
    if( *data <= 200 )
    {
        currentLevel = *data;
        nextBlinkTime.reset();
        setLogicalState( *data ? ON  : OFF );
    }
    else if( isKeyFeedbackOnCmd( *data ) )
    {
        pwmOutput->setDutyCycle();
        feedbackCmdActive = true;
        return; // no logging for feedbackCmd
    }
    else if( isKeyFeedbackOffCmd( *data ) )
    {
        feedbackCmdActive = false;
        return; // no logging for feedbackCmd
    }

    if( length >= 6 )
    {
        offLevel = data[1];
        onLevel = data[2];
        blinkOnTime = data[3];
        blinkOffTime = data[4];
        blinkQuantity = data[5];
    }
    if( isBlinkOnCmd( *data ) )
    {
        nextBlinkTime = Timestamp();
        setLogicalState( BLINK_ON );
    }
    else if( isBlinkToggleCmd( *data ) )
    {
       if( logicalState != BLINK_ON )
       {
         nextBlinkTime = Timestamp();
         setLogicalState( BLINK_ON );
       }
       else
       {
         currentLevel = offLevel;
         nextBlinkTime.reset();
         setLogicalState( OFF );
       }
    }
    else if( isToggleCmd( *data ) )
    {
        if( isLogicalOn() )
        {
            currentLevel = offLevel;
            setLogicalState( OFF );
        }
        else
        {
            currentLevel = onLevel;
            setLogicalState( ON );
        }
        nextBlinkTime.reset();
    }
    else // toggle
    {   
        if( currentLevel ) 
        {
            currentLevel = 0;
        }
        else
        {
            currentLevel = 200;
        }
        nextBlinkTime.reset();
    }

    // Logging
    if( !nextFeedbackTime.isValid() && config->logging ) 
    {
        nextFeedbackTime = Timestamp();
        nextFeedbackTime += (device->getLoggingTime() * 100);
    }
};


uint8_t HBWDimmer::get(uint8_t* data) 
{
    // map 0-100% to 0-200
    (*data) = currentLevel;
    return 1;
};

void HBWDimmer::loop(HBWDevice* device, uint8_t channel) 
{
    if( nextBlinkTime.isValid() && nextBlinkTime.since() )
    {
        // handle blinking
        if( 2*pwmOutput->getDutyCycle() == onLevel )
        {
            // is ON
            nextBlinkTime += (blinkOffTime * 100);
            pwmOutput->setDutyCycle( offLevel/2 );
        }
        else
        {
            // is OFF
            if( blinkQuantity )
            {
                nextBlinkTime += (blinkOnTime * 100);
                pwmOutput->setDutyCycle( onLevel/2 );

                if( blinkQuantity != 255 )
                {
                    blinkQuantity--;
                }
            }
            else
            {
                nextBlinkTime.reset();
            }
        }

    }
    if( !feedbackCmdActive && !nextBlinkTime.isValid() )
    {
        // the default range is 0-200, this must be mapped to 0-100% duty cycle
        pwmOutput->setDutyCycle( currentLevel/2 );
    }

    // feedback trigger set?
    if( !nextFeedbackTime.isValid() ) 
    {
        return;
    }

    if( !nextFeedbackTime.since() ) 
    {
        return;
    }
    
    uint8_t level;
    uint8_t errcode = device->sendInfoMessage(channel, get(&level), &level);

    // sendInfoMessage returns 0 on success, 1 if bus busy, 2 if failed
    if( errcode ) 
    {  
        // bus busy
        // try again later, but insert a small delay
        nextFeedbackTime += 250;
    }
    else
    {
        nextFeedbackTime.reset();
    }
}