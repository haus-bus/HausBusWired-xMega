/* 
* PwmOutput.h
*
* Created: 03.12.2016 01:56:53
* Author: haus-bus
*/


#ifndef __PWMOUTPUT_H__
#define __PWMOUTPUT_H__

#include <pwm.h>

class PwmOutput
{
//variables
public:
protected:
private:
	pwm_config config;

//functions
public:
	inline PwmOutput( pwm_tc_t tc,	pwm_channel_t channel, uint16_t freq_hz )
	{
		pwm_init( &config, tc, channel, freq_hz );
		pwm_start( &config, 0 );
	}
	  
	uint8_t getDutyCycle();
	    
	void setDutyCycle(uint8_t duty = 100);
	
	inline void clear() { setDutyCycle(0); }
		
protected:
private:

}; //PwmOutput

#endif //__PWMOUTPUT_H__
