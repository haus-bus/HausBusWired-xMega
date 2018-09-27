/*
 * DebugOptions.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef DebugOptions_H
#define DebugOptions_H

#include <DefaultTypes.h>

class DebugOptions
{
   public:

      enum OPTIONS
      {
         SEND_TRIGGERED_RULE_EVENT = 0x01,
         GATEWAYS_READ_ONLY = 0x02,
         SEND_MSG_LOAD_EVENT = 0x04
      };

      ////    Operations    ////

      inline static uint8_t gatewaysReadOnly()
      {
         return options & GATEWAYS_READ_ONLY;
      }

      inline static uint8_t notifyTriggeredRule()
      {
         return options & SEND_TRIGGERED_RULE_EVENT;
      }

      inline static uint8_t notifyMessageLoad()
      {
         return options & SEND_MSG_LOAD_EVENT;
      }

      inline static void set( uint8_t _options )
      {
         options = _options;
      }

      ////    Additional operations    ////

   private:

      inline static uint8_t getOptions()
      {
         return options;
      }

      inline static void setOptions( uint8_t p_options )
      {
         options = p_options;
      }

      ////    Attributes    ////

      static uint8_t options;
};

#endif

