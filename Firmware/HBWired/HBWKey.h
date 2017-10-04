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
            INPUTTYPE_MASK = 0x07,
            UNLOCKED_MASK = 0x08,
            ACTIVE_LOW_MASK = 0x10,
            FEEDBACK_MASK = 0x80,
         };

         uint8_tx options;
         uint8_tx long_press_time;

         public:

            enum InputType
            {
               PUSHBUTTON = 0,
               SWITCH,
               MAX_INPUTTYPE
            };

            inline uint8_t getInputType() const
            {
               return ( options & INPUTTYPE_MASK );
            }

            inline void setInputType( uint8_t type )
            {
               if ( type < MAX_INPUTTYPE )
               {
                  options.update( ( options & ~INPUTTYPE_MASK ) | type );
               }
            }

            inline bool isPushButton() const
            {
               return getInputType() == PUSHBUTTON;
            }

            inline bool isSwitch() const
            {
               return getInputType() == SWITCH;
            }

            inline bool isUnlocked() const
            {
               return options & UNLOCKED_MASK;
            }

            inline bool isActiveLow() const
            {
               return options & ACTIVE_LOW_MASK;
            }

            inline bool isFeedbackEnabled() const
            {
               return options & FEEDBACK_MASK;
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

      HBWKey( PortPin _pin, Config* _config, HBWChannel* _feedbackChannel = NULL );

      inline void setFeedbackChannel( HBWChannel* _feedbackChannel )
      {
         feedbackChannel = _feedbackChannel;
      }

      virtual void loop( HBWDevice*, uint8_t channel );
      virtual void checkConfig();

   protected:

      void resetChannel();

      void handlePushButtonSignal( HBWDevice* device, uint8_t channel );

      void handleSwitchSignal( HBWDevice* device, uint8_t channel );

   private:
      uint8_t keyPressNum;
      Config* config;
      HBWChannel* feedbackChannel;
      DigitalInput digitalIn;


      Timestamp keyPressedTimestamp;   // Zeit, zu der die Taste gedrueckt wurde (fuer's Entprellen)
      Timestamp lastSentLong;        // Zeit, zu der das letzte Mal longPress gesendet wurde
};

#endif