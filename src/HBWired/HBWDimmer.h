/* 
* HBWDimmer.h
*
* Created: 26.04.2017 09:01:56
* Author: viktor.pankraz
*/


#ifndef __HBWDIMMER_H__
#define __HBWDIMMER_H__

#include "Elements/PwmOutput.h"
#include "HBWired.h"


class HBWDimmer : public HBWChannel
{
//variables
public:
	struct Config 
    {
		uint8_t logging:1;              // 0x0000
		uint8_t        :7;              // 0x0000
		uint8_t dummy;
	};

protected:
private:
    Config* config; 
    PwmOutput* pwmOutput;
    uint32_t lastFeedbackTime;  
    uint16_t nextFeedbackDelay; 

//functions
public:
	HBWDimmer( PwmOutput* _pwmOutput, Config* _config );

    // definition of needed functions from HBWChannel class
    virtual uint8_t get(uint8_t* data);
    virtual void loop(HBWDevice*, uint8_t channel);
    virtual void set(HBWDevice*, uint8_t length, uint8_t const * const data);

protected:
private:


}; //HBWDimmer

#endif //__HBWDIMMER_H__
