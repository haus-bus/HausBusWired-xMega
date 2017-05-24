
#ifndef HBWKey_h
#define HBWKey_h
#include <inttypes.h>
#include "Arduino.h"
#include "HBWired.h"



// Class HBWKey
class HBWKey : public HBWChannel 
{
	public:

        struct Config 
        {
            uint8_t isPushButton:1;
            uint8_t isUnlocked:1;
            uint8_t :5;
            uint8_t ledFeedbackEnabled:1;
            uint8_t long_press_time;
        };

		HBWKey(uint8_t _pin, Config* _config, HBWChannel* _feedbackChannel = NULL);

		virtual void loop(HBWDevice*, uint8_t channel);

	private:
  	    uint8_t pin;   // Pin
		uint8_t keyPressNum;
		Config* config;
        HBWChannel* feedbackChannel;

        uint32_t keyPressedMillis;  // Zeit, zu der die Taste gedrueckt wurde (fuer's Entprellen)
        uint32_t lastSentLong;      // Zeit, zu der das letzte Mal longPress gesendet wurde
};

#endif 