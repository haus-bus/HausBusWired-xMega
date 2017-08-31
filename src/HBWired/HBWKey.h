#ifndef HBWKey_h
#define HBWKey_h

#include "HBWired.h"
#include <DigitalInput.h>


// Class HBWKey
class HBWKey : public HBWChannel
{
   public:

      class Config
      {
         enum OptionMask
         {
            PUSHBUTTON_MASK = 0x01,
            UNLOCKED_MASK = 0x02,
         };

         uint8_tx options;
         uint8_tx long_press_time;

         public:
            inline bool isPushButton() const
            {
               return options & PUSHBUTTON_MASK;
            }
            inline bool isUnlocked() const
            {
               return options & UNLOCKED_MASK;
            }
            inline uint8_t getLongPressTime() const
            {
               return long_press_time;
            }

            inline void setLongPressTime( uint8_t time )
            {
               long_press_time.update( time );
            }


      };

      HBWKey( PortPin _pin, Config* _config, HBWChannel* _feedbackChannel = NULL, bool _inverted = false );

      inline void setFeedbackChannel( HBWChannel* _feedbackChannel )
      {
         feedbackChannel = _feedbackChannel;
      }

      virtual void loop( HBWDevice*, uint8_t channel );
      virtual void checkConfig();

   private:
      uint8_t keyPressNum;
      Config* config;
      HBWChannel* feedbackChannel;
      DigitalInput digitalIn;

      uint32_t keyPressedMillis;    // Zeit, zu der die Taste gedrueckt wurde (fuer's Entprellen)
      uint32_t lastSentLong;        // Zeit, zu der das letzte Mal longPress gesendet wurde
};

#endif