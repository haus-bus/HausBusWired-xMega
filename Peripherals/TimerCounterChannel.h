/*
 * TimerCounterChannel.h
 *
 * Created: 12.08.2018 22:50:05
 * Author: Viktor Pankraz
 */


#ifndef __TIMERCOUNTERCHANNEL_H__
#define __TIMERCOUNTERCHANNEL_H__

#include "TimerCounter.h"

class TimerCounterChannel
{
// variables
   public:
   protected:
   private:
      TimerCounter* timerCounter;
      TimerCounter::Channel channel;

// functions
   public:
      TimerCounterChannel( TimerCounter* _timerCounter, TimerCounter::Channel _channel ) :
         timerCounter( _timerCounter ),
         channel( _channel )
      {
      }

      inline uint16_t getCurrentCount()
      {
         return timerCounter->getCount();
      }

      inline void clearCCFlag()
      {
         timerCounter->clearCCFlag( channel );
      }

      inline uint8_t getCCFlag()
      {
         return timerCounter->getCCFlag( channel );
      }

      inline uint16_t getCapture()
      {
         return timerCounter->getCapture( channel );
      }

      inline void disable()
      {
         timerCounter->disableChannel( channel );
      }

      inline void enable()
      {
         timerCounter->enableChannel( channel );
      }

      inline void setCompare( uint16_t value )
      {
         timerCounter->setCompare( channel, value );
      }

      inline void setCCIntLevel( TC_CCCINTLVL_t intLevel )
      {
         timerCounter->setCCIntLevel( channel, intLevel );
      }

   protected:
   private:

}; // TimerCounterChannel

#endif // __TIMERCOUNTERCHANNEL_H__
