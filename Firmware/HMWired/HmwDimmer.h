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
            LOGGING_MASK   = 0x80,

            DIMMING_MODE_MASK = 0x03,
         };

         uint8_tx options;
         uint8_tx reserve;

         public:

            enum Mode
            {
               SWITCH = 0,
               DIMM_L,
               DIMM_CR
            };

            inline bool isLogging() const
            {
               return options & LOGGING_MASK;
            }

            inline uint8_t getDimmingMode() const
            {
               return ( options & DIMMING_MODE_MASK );
            }
      };

      static const uint8_t MAX_LEVEL = 200;
      static const uint8_t NORMALIZE_LEVEL = 8;
      static const uint16_t MAX_LEVEL_PERIOD = MAX_LEVEL * 100 / NORMALIZE_LEVEL;

   protected:
   private:
      Config* config;
      PwmOutput pwmOutput;
      uint8_t currentLevel;
      Timestamp nextFeedbackTime;

// functions
   public:
      HmwDimmer( PortPin _portPin, Config* _config );

      // definition of needed functions from HBWChannel class
      virtual uint8_t get( uint8_t* data );
      virtual void loop( uint8_t channel );
      virtual void set( uint8_t length, uint8_t const* const data );
      virtual void checkConfig();

   protected:

      void setLevel( uint8_t );

      uint8_t getLevel() const;

   private:


}; // HmwDimmer

#endif // __HMWDIMMER_H__
