/*
 * HmwLed.h
 *
 * Created: 26.04.2017 09:01:56
 * Author: viktor.pankraz
 */


#ifndef __HMWLED_H__
#define __HMWLED_H__

#include "HmwChannel.h"
#include <PwmOutput.h>
#include <Time/Timestamp.h>
#include <xEeprom.h>


class HmwLed : public HmwChannel
{

   public:
      class Config
      {
         enum OptionMask
         {
            LOGGING_MASK = 0x80,
            PWM_RANGE_MASK = 0x7F
         };

         uint8_tx options;

         uint8_tx reserve;

         public:
            inline bool isLogging() const
            {
               return options & LOGGING_MASK;
            }

            inline uint8_t getPwmRange()
            {
               return ( options & PWM_RANGE_MASK );
            }

            inline void setPwmRange( uint8_t value )
            {
               options.update( ( options & ~PWM_RANGE_MASK ) | ( value & PWM_RANGE_MASK ) );
            }
      };

      static const uint8_t MAX_LEVEL = 200;
      static const uint8_t NORMALIZE_LEVEL = 8;
      static const uint16_t MAX_LEVEL_PERIOD = MAX_LEVEL * 100 / NORMALIZE_LEVEL;

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

      const uint8_t defaultPwmRange;

// functions
   public:
      HmwLed( PortPin _portPin, Config* _config, bool _inverted = false, uint8_t _defaultPwmRange = 100 );

      // definition of needed functions from HBWChannel class
      virtual uint8_t get( uint8_t* data );
      virtual void loop( uint8_t channel );
      virtual void set( uint8_t length, uint8_t const* const data );
      virtual void checkConfig();

   protected:

      void setLevel( uint8_t );

      uint8_t getLevel() const;

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

}; // HmwLed

#endif // __HMWLED_H__
