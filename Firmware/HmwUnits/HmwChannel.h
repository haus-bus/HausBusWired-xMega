/*
 * HmwChannel.h
 *
 * Created: 18.11.2017 23:25:12
 * Author: viktor.pankraz
 */


#ifndef __HMWCHANNEL_H__
#define __HMWCHANNEL_H__

#ifndef MAX_CHANNELS
#define MAX_CHANNELS 34
#endif


#include <stdio.h>

class HmwChannel
{
// variables
   public:
      enum LedCommands
      {
         ON = 201,
         OFF,
         TOGGLE,
         BLINK_ON,
         BLINK_TOGGLE,
         KEY_FEEDBACK_ON,
         KEY_FEEDBACK_OFF,
      };

      enum GenericChannelCommands
      {

         LINK_ACTION = 254,
         BROADCAST_LINK_ACTION = 255,
      };

      enum Type
      {
         UNKNOWN,
         HMW_KEY,
         HMW_LED,
         HMW_DIMMER,
         HMW_DS18X20,
         HMW_SHTC3
      };

   protected:

      Type type;
      uint8_t channelId;
      HmwChannel();

   private:
      static uint8_t numChannels;
      static HmwChannel* instances[MAX_CHANNELS];

// functions
   public:

      static inline uint8_t getNumChannels()
      {
         return numChannels;
      }

      static inline HmwChannel* getChannel( uint8_t channel )
      {
         return instances[channel];
      }

      inline bool isOfType( Type _type )
      {
         return type == _type;
      }

      inline void disable()
      {
         nextActionDelay = 0;
      }

      inline void enable()
      {
         nextActionDelay = 1;
      }

      uint32_t convertToTime( uint16_t value );

      virtual void set( uint8_t length, uint8_t const* const data );
      virtual uint8_t get( uint8_t* data );  // returns length, data must be big enough
      virtual void loop( uint8_t channel );  // channel for feedbacks etc.
      virtual void checkConfig();

   protected:

      uint16_t nextActionDelay;

   private:


}; // HmwChannel

#endif // __HMWCHANNEL_H__
