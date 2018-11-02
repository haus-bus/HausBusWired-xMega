/*
 * SlotHw.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_HwUnitBoards_SlotHw_H
#define Electronics_HwUnitBoards_SlotHw_H

#include "HwUnitBoards.h"
#include <DigitalOutput.h>
#include <IDimmerHw.h>
#include <Peripherals/TimerCounter.h>
class Logger;


class SlotHw : public IDimmerHw
{
   public:

      enum SlotType {
         UNUSED_SLOT,
         DIMMER,
         POWER_SOCKET,
         ROLLER_SHUTTER,
         DOUBLE_SWITCH = 0x07,
         DIMMER_30 = 0x0A,
         DIMMER_31 = 0x0B,
         MAX_SLOT_TYPES
      };

      enum Versions {
         V30 = 30,
         V31 = 31
      };

      enum Modes {
         DIMM_CR = 0,
         DIMM_L = 1,
         SWITCH = 2
      };

      ////    Constructors and destructors    ////

      SlotHw();

      ////    Operations    ////

      void configure( uint8_t targetType );

      static void configureOutput( DigitalOutput& output, uint8_t isOutput );

      uint16_t getPwmDuty();

      uint8_t hasError();

      inline uint8_t isDimmerHw();

      inline bool isDoubleSwitchHw();

      uint16_t isOn();

      inline bool isPowerSocketHw();

      inline bool isRollerShutterHw();

      void on( uint16_t value );

      virtual uint8_t setMode( uint8_t mode );

      void setPwmDuty( uint16_t duty );

      void stop();

      virtual uint8_t suspend( uint8_t mode );

      ////    Additional operations    ////

      inline TimerCounter* getTimerCounter0() const
      {
         return timerCounter0;
      }

      inline void setTimerCounter0( TimerCounter* p_timerCounter0 )
      {
         timerCounter0 = p_timerCounter0;
      }

      inline uint8_t getType() const
      {
         return type;
      }

      inline void setType( uint8_t p_type )
      {
         type = p_type;
      }

      inline uint8_t getPwmEnabled() const
      {
         return pwmEnabled;
      }

      inline void setPwmEnabled( uint8_t p_pwmEnabled )
      {
         pwmEnabled = p_pwmEnabled;
      }

      DigitalOutput* getDigitalOutput0() const;

      DigitalOutput* getDigitalOutput1() const;

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      ////    Attributes    ////

      static const uint8_t debugLevel;

   public:

      TimerCounter* timerCounter0;

      uint8_t type : 4;

      uint8_t pwmEnabled : 1;

      ////    Relations and components    ////

   protected:

      DigitalOutput digitalOutput0;

      DigitalOutput digitalOutput1;
};

inline uint8_t SlotHw::isDimmerHw()
{
   if ( type == DIMMER )
   {
      return true;
   }
   if ( type == DIMMER_30 )
   {
      return V30;
   }
   if ( type == DIMMER_31 )
   {
      return V31;
   }
   return false;
}

inline bool SlotHw::isDoubleSwitchHw()
{
   return ( type == DOUBLE_SWITCH );
}

inline bool SlotHw::isPowerSocketHw()
{
   return ( type == POWER_SOCKET );
}

inline bool SlotHw::isRollerShutterHw()
{
   return ( type == ROLLER_SHUTTER );
}

#endif
