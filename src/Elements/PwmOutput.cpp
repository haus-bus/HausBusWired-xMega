/* 
* PwmOutput.cpp
*
* Created: 03.12.2016 01:56:53
* Author: haus-bus
*/


#include "PwmOutput.h"

void PwmOutput::setDutyCycle( uint8_t dutyCycle ) 
{
	pwm_set_duty_cycle_percent( &config, dutyCycle );
    currentDutyCycle = dutyCycle;
}