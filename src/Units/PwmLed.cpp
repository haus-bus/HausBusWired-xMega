/* 
* Led.cpp
*
* Created: 03.12.2016 03:29:52
* Author: haus-bus
*/


#include "PwmLed.h"

uint8_t PwmLed::ledCount = 0;

PwmLed* PwmLed::instances[PWM_LED_COUNT];

PwmLed::PwmLed( pwm_tc_t tc,	pwm_channel_t channel, uint16_t freq_hz ) : PwmOutput( tc, channel, freq_hz )
{
	instances[ledCount] = this;
	setId( (ClassId::PWM_LED << 8 ) | ++ledCount );
	
} //Led

PwmLed* PwmLed::isPwmLed( Object obj )
{
	if( obj.isClassId(ClassId::PWM_LED) && ( obj.getInstanceId() < ledCount ) )
	{
		return instances[obj.getInstanceId()];
	}
	return NULL;
}