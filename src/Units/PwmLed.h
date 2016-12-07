/* 
* Led.h
*
* Created: 03.12.2016 03:29:53
* Author: haus-bus
*/


#ifndef __PWM_LED_H__
#define __PWM_LED_H__

#include "Object.h"
#include <Elements/PwmOutput.h>
#include <board.h>

class PwmLed : public Object, public PwmOutput
{
//variables
public:
	static uint8_t ledCount;
	
protected:
	static PwmLed* instances[PWM_LED_COUNT];
	
private:

//functions
public:
	PwmLed( pwm_tc_t tc, pwm_channel_t channel, uint16_t freq_hz );
	
	static PwmLed* isPwmLed( Object obj );
	
protected:
private:

}; //Led

#endif //__PWM_LED_H__
