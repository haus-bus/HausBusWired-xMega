/*
 * RuleElement.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "RuleElement.h"

#include <Traces/Logger.h>
#include <Time/Calender.h>

const uint8_t RuleElement::debugLevel( DEBUG_LEVEL_OFF );

void RuleElement::getAction( RuleElement::Action* _action, uint8_t index ) const
{
  uintptr_t address = reinterpret_cast<uintptr_t>( &condition )
      + getNumOfConditions() * sizeof(Condition) + index * sizeof(Action);
  ApplicationTable::read( address, _action, sizeof(Action) );
}

void RuleElement::getCondition( RuleElement::Condition* _condition,
                                uint8_t index ) const
{
  ApplicationTable::read( reinterpret_cast<uintptr_t>( &condition[index] ),
                          _condition, sizeof(Condition) );
}

uint16_t RuleElement::getLength() const
{
  uint16_t length = getNumOfConditions();

  if ( length )
  {
    length = length * sizeof(Condition) + getNumOfActions() * sizeof(Action)
        + 8;
  }
  return length;
}

uint8_t RuleElement::getNextState() const
{
  return ApplicationTable::read( reinterpret_cast<uintptr_t>( &nextState ) );
}

uint8_t RuleElement::getNumOfActions() const
{
  return ApplicationTable::read( reinterpret_cast<uintptr_t>( &numOfActions ) );
}

uint8_t RuleElement::getNumOfConditions() const
{
  uint8_t value = ApplicationTable::read(
      reinterpret_cast<uintptr_t>( &numOfConditions ) );
  if ( value == 0xFF )
  {
    return 0;
  }
  return value;
}

uint8_t RuleElement::getState() const
{
  return ApplicationTable::read( reinterpret_cast<uintptr_t>( &state ) );
}

void RuleElement::getTimeCondition(
    RuleElement::TimeCondition* _condition ) const
{
  ApplicationTable::read( reinterpret_cast<uintptr_t>( &startTime ), _condition,
                          sizeof(TimeCondition) );
}

bool RuleElement::isActiveForState( uint8_t _state ) const
{
  uint8_t triggerState = getState();
  triggerState = ((triggerState == _state) || (triggerState == EVERY_STATE));
  if ( triggerState )
  {
    TimeCondition timeCondition;
    getTimeCondition( &timeCondition );

    WeekTime now = Calender::getCurrentWeekTime();
    uint8_t specialTimeCondition = (timeCondition.startTime & 0xFF);
    if ( (specialTimeCondition >= WeekTime::MAX_MINUTES)
        && (specialTimeCondition != 0xFF) )
    {
      DEBUG_M2( FSTR("specialCondition "), specialTimeCondition );
      if ( specialTimeCondition & DAYLY_MASK )
      {
        uint8_t startDay = ((WeekTime*)(&timeCondition.startTime))->getWeekDay();
        uint8_t endDay = ((WeekTime*)(&timeCondition.endTime))->getWeekDay();
        uint8_t weekDay = now.getWeekDay();
        if ( startDay <= endDay )
        {
          if( (weekDay < startDay) || (weekDay > endDay) )
          {
            return false;
          }
        }
        else if ( startDay > endDay )
        {
          if( ((weekDay < startDay) && (weekDay > endDay)) )
          {
            return false;
          }
        }
        timeCondition.startTime &= ~DAYLY_MASK;
        timeCondition.startTime |= WeekTime::WEEK_DAY_MASK;
        timeCondition.endTime |= WeekTime::WEEK_DAY_MASK;
      }
      else
      {
        bool active = (((specialTimeCondition == DAY_ONLY) && now.isDay())
            || ((specialTimeCondition == NIGHT_ONLY) && !now.isDay()));
        if ( !active )
        {
          return false;
        }
        timeCondition.startTime |= ~WeekTime::WEEK_DAY_MASK;
        if ( (timeCondition.endTime & WeekTime::WEEK_DAY_MASK)
            == WeekTime::WEEK_DAY_MASK )
        {
          timeCondition.endTime = timeCondition.startTime;
        }
      }
    }

    DEBUG_M4( FSTR("startTime "), timeCondition.startTime, FSTR(" endTime "), timeCondition.endTime );
    if ( (timeCondition.startTime == 0xFFFF)
        || (timeCondition.endTime == 0xFFFF) )
    {
      return true;
    }
    WeekTime startTime (timeCondition.startTime);
    WeekTime endTime (timeCondition.endTime);
    return now.isInRange( startTime, endTime );
  }
  return false;
}

bool RuleElement::isAnyConditionActive(
    const HACF::ControlFrame& message ) const
{
  Condition condition;

  uint32_t senderId = message.senderId.getId();
  uint8_t index = getNumOfConditions();
  while ( index-- )
  {
    getCondition( &condition, index );
    if ( senderId == condition.senderId )
    {
      DEBUG_M2( FSTR("sender "), senderId );DEBUG_M2( FSTR("c:"), index );
      uint8_t length = message.getDataLength() - 1;
      while ( (message.data[length] == condition.data[length])
          || (condition.data[length] == WILDCARD) )
      {
        DEBUG_L2( ' ', condition.data[length] );
        if ( length == 0 )
        {
          return true;
        }
        length--;
      }
    }
  }
  return false;
}

RuleElement* RuleElement::next() const
{
  uint16_t length = getLength();
  if ( length != 0 )
  {
    return reinterpret_cast<RuleElement*>( reinterpret_cast<uintptr_t>( this )
        + length );
  }
  return 0;
}
