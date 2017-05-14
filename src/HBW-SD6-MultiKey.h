/*
 * HBW_SD6_MultiKey.h
 *
 * Created: 15.05.2017 00:29:36
 *  Author: viktor.pankraz
 */ 


#ifndef HBW_SD6_MULTIKEY_H_
#define HBW_SD6_MULTIKEY_H_

#include "HBWired/HBWired.h"
#include "HBWired/HBWKey.h"
#include "HBWired/HBWDimmer.h"
#include "HBWired/HBWDS1820.h"
#include "HBWired/HBWAnalogIn.h"

// The HBWired-Device
class HBWSD6Multikey : public HBWDevice 
{
    public:
    inline HBWSD6Multikey(uint8_t _devicetype, uint8_t _hardware_version, uint16_t _firmware_version,
    Stream* _rs485, uint8_t _txen,
    uint8_t _configSize, void* _config,
    uint8_t _numChannels, HBWChannel** _channels,
    Stream* _debugstream, HBWLinkSender* _linkSender = NULL, HBWLinkReceiver* _linkReceiver = NULL) 
        : HBWDevice( _devicetype, _hardware_version, _firmware_version, _rs485,  _txen, _configSize, _config, _numChannels, _channels,  _debugstream, _linkSender, _linkReceiver )
        {
            afterReadConfig();
        }
    
    virtual void afterReadConfig();

    private:
    inline void checkAndCorrectConfig( HBWDS1820::Config* config)
    {
        if( config->minDelta > 250 )
        {
            config->minDelta = 5;
        }
        if( config->minInterval && ( ( config->minInterval < 5 ) || ( config->minInterval > 3600 ) ) )
        {
            config->minInterval = 10;
        }
        if( config->maxInterval && ( ( config->maxInterval < 5 ) || ( config->maxInterval > 3600 ) ) )
        {
            config->maxInterval = 150;
        }
        if( config->maxInterval && ( config->maxInterval < config->minInterval ) )
        {
            config->maxInterval = 0;
        }
    }

    inline void checkAndCorrectConfig( HBWKey::Config* config)
    {
        if( ( config->long_press_time < 4 ) || ( config->long_press_time > 50 ) )
        {
            config->long_press_time = 10;
        }
    }

    inline void checkAndCorrectConfig( HBWDimmer::Config* config)
    {
        // nothing to do 
    }
};

#endif /* HBW_SD6_MULTIKEY_H_ */