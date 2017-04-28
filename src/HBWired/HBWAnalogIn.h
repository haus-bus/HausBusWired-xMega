/*
 * HBWAnalogIn.h
 *
 *  Created on: 26.04.2017
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_HBWAnalogIn_H
#define HwUnits_HBWAnalogIn_H

#include <Utils/Timestamp.h>

#include "HBWired.h"


class HBWAnalogIn : public HBWChannel
{
public:

    enum State
    {  
        INIT_ADC,
        START_MEASUREMENT,
        SAMPLE_VALUES, 
        SEND_FEEDBACK
    };

  	struct Config
  	{
      	uint8_t     unused1;           
      	uint8_t     minDelta;
        uint8_t     unused2;
        uint16_t    minInterval;
        uint16_t    maxInterval;
        uint8_t     unused3[9]; 
  	};


  ////    Constructors and destructors    ////

  HBWAnalogIn( ADC_t* _adc, uint8_t _adcChannel, Config* _config );

  ////    Operations    ////

    // definition of needed functions from HBWChannel class
    virtual uint8_t get(uint8_t* data);
    virtual void loop(HBWDevice*, uint8_t channel);

private:


  ////    Additional operations    ////

public:


private:


  ////    Attributes    ////

public:

private:

    ADC_t* adc;
    
    uint8_t adcChannel;

    Config* config;

    State state;

    uint16_t nextActionDelay;

    uint16_t currentValue;

    uint16_t lastSentValue;

    Timestamp lastActionTime;
    
    Timestamp lastSentTime;

};


#endif

