/*
 * HmwDimmer.h
 *
 * Created: 26.04.2017 09:01:56
 * Author: viktor.pankraz
 */


#ifndef __HMWDIMMER_H__
#define __HMWDIMMER_H__

#include "HmwChannel.h"
#include <PwmOutput.h>
#include <Time/Timestamp.h>
#include <xEeprom.h>


class HmwDimmer : public HmwChannel
{

   public:
      class Config
      {
         enum OptionMask
         {
            LOGGING_MASK = 0x80,
            RESERVED_MASK = 0x7C,
            DIMMING_MODE_MASK = 0x03,
            DEFAULT_OPTIONS = 0x83,
         };

         uint8_tx options;
         uint8_tx reserve;

         public:

            enum Mode
            {
               SWITCH = 0,
               DIMM_PWM,
               DIMM_LEADING,
               DIMM_TRAILING
            };

            inline bool isLogging() const
            {
               return options & LOGGING_MASK;
            }

            inline uint8_t getDimmingMode() const
            {
               return ( options & DIMMING_MODE_MASK );
            }

            inline bool isDimmingModeSwitch() const
            {
               return ( getDimmingMode() == SWITCH );
            }

            inline bool isDimmingModeLeading() const
            {
               return ( getDimmingMode() == DIMM_LEADING );
            }

            inline bool isDimmingModeTrailing() const
            {
               return ( getDimmingMode() == DIMM_TRAILING );
            }

            inline bool isDimmingModePwm() const
            {
               return ( getDimmingMode() == DIMM_PWM );
            }

            inline void checkOrRestore()
            {
               if ( ( options & RESERVED_MASK ) != RESERVED_MASK )
               {
                  options.update( DEFAULT_OPTIONS | RESERVED_MASK );
               }
               if ( reserve != 0xFF )
               {
                  reserve.update( 0xFF );
               }
            }
      };

      enum Actions
      {
         INACTIVE,
         JUMP_TO_TARGET,
         TOGGLE_TO_COUNTER,
         TOGGLE_INVERS_TO_COUNTER,
         UPDIM,
         DOWNDIM,
         TOGGLEDIM,
         TOGGLEDIM_TO_COUNTER,
         TOGGLEDIM_INVERS_TO_COUNTER,
      };

      enum TimeModes
      {
         ABSOLUTE,
         MINIMAL,
      };

      struct ActionParameter
      {
         uint8_t actionType    : 4;
         uint8_t multiExecute  : 1;
         uint8_t onDelayMode   : 1;
         uint8_t offTimeMode   : 1;
         uint8_t onTimeMode    : 1;
         uint8_t offLevel;
         uint8_t onMinLevel;
         uint8_t onLevel;
         uint8_t rampStartStep;
         uint8_t offDelayStep;
         uint16_t onDelayTime;
         uint16_t rampOnTime;
         uint16_t onTime;
         uint16_t offDelayTime;
         uint16_t rampOffTime;
         uint16_t offTime;
         uint8_t dimMinLevel;
         uint8_t dimMaxLevel;
         uint8_t dimStep;
         uint8_t jtOnDelay  : 4;
         uint8_t jtRampOn   : 4;
         uint8_t jtOn       : 4;
         uint8_t jtOffDelay : 4;
         uint8_t jtRampOff  : 4;
         uint8_t jtOff      : 4;
      };

      struct LinkCommand
      {
         uint8_t keyNum;
         ActionParameter* actionParameter;
      };

      static const uint8_t MAX_LEVEL = 200;
      static const uint8_t DEFAULT_NORMALIZE_LEVEL = 205;
      static const uint8_t LOOP_PERIOD_MS = 8;
      static const uint16_t MAX_NEXT_ACTION_TIME = 0xC000;

   protected:

      enum States
      {
         JT_ONDELAY = 0x00,
         JT_RAMP_ON = 0x01,
         JT_ON = 0x02,
         JT_OFFDELAY = 0x03,
         JT_RAMP_OFF = 0x04,
         JT_OFF = 0x05,
         JT_NO_JUMP_IGNORE_COMMAND = 0x06,
         START_UP
      };

   private:

      static const uint8_t debugLevel;

      const uint8_t normalizeLevel;

      PwmOutput pwmOutput;

      DigitalOutput enableOutput;

      uint8_t currentLevel;

      Config* config;

      ActionParameter const* actionParameter;

      Timestamp nextFeedbackTime;

      Timestamp nextActionTime;

      States state;

      uint8_t rampStep;

      uint8_t lastKeyNum;

      uint32_t nextStepTime;


// functions
   public:
      HmwDimmer( PortPin _pwmPin, PortPin _enablePin, Config* _config, uint8_t _normalizeLevel = DEFAULT_NORMALIZE_LEVEL );

      // definition of needed functions from HBWChannel class
      virtual uint8_t get( uint8_t* data );
      virtual void loop( uint8_t channel );
      virtual void set( uint8_t length, uint8_t const* const data );
      virtual void checkConfig();

   protected:

      void setLevel( uint8_t );

   private:

      void calculateRampParameter();

      void jumpToNextState( uint8_t nextState );

      uint8_t getNextDimLevel( bool dimUp );

      inline void setMainState( States _state )
      {
         if ( state == _state )
         {
            // we stay in the same state, stop loop
            nextActionTime.reset();
         }
         state = _state;
      }

      void handleStateChart( bool fromMainLoop = true );

      bool inline isValidActionTime( uint16_t time )
      {
         return ( time && ( time < MAX_NEXT_ACTION_TIME ) );
      }


}; // HmwDimmer

#endif // __HMWDIMMER_H__
