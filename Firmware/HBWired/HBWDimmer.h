/*
 * HBWDimmer.h
 *
 * Created: 26.04.2017 09:01:56
 * Author: viktor.pankraz
 */


#ifndef __HBWDIMMER_H__
#define __HBWDIMMER_H__

#include "HBWired.h"
#include <PwmOutput.h>


class HBWDimmer : public HBWChannel
{

   public:
      class Config
      {
         enum OptionMask
         {
            LOGGING_MASK = 0x01,
         };

         uint8_tx options;
         uint8_tx reserved;

         public:
            inline bool isLogging() const
            {
               return options & LOGGING_MASK;
            }
      };

      static const uint8_t MAX_LEVEL = 200;

   protected:
   private:
      Config* config;
      PwmOutput pwmOutput;
      bool feedbackCmdActive;
      uint8_t currentLevel;
      uint8_t onLevel;
      uint8_t offLevel;
      uint8_t blinkOnTime;
      uint8_t blinkOffTime;
      uint8_t blinkQuantity;
      Timestamp nextFeedbackTime;
      Timestamp nextBlinkTime;
      uint8_t logicalState;

// functions
   public:
      HBWDimmer( PortPin _portPin, Config* _config, bool _inverted = false );

      // definition of needed functions from HBWChannel class
      virtual uint8_t get( uint8_t* data );
      virtual void loop( HBWDevice*, uint8_t channel );
      virtual void set( HBWDevice*, uint8_t length, uint8_t const* const data );

   protected:

   private:
      inline bool isToggleCmd( uint8_t cmd )
      {
         return cmd == TOGGLE;
      }

      inline bool isBlinkOnCmd( uint8_t cmd )
      {
         return cmd == BLINK_ON;
      }

      inline bool isBlinkToggleCmd( uint8_t cmd )
      {
         return cmd == BLINK_TOGGLE;
      }

      inline bool isKeyFeedbackOnCmd( uint8_t cmd )
      {
         return cmd == KEY_FEEDBACK_ON;
      }

      inline bool isKeyFeedbackOffCmd( uint8_t cmd )
      {
         return cmd == KEY_FEEDBACK_OFF;
      }

      inline bool isLogicalOn( void )
      {
         return logicalState != OFF;
      }

      inline void setLogicalState( uint8_t state )
      {
         logicalState = state;
      }

}; // HBWDimmer

#endif // __HBWDIMMER_H__
