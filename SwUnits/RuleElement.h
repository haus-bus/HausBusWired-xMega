/*
 * RuleElement.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef RuleElement_H
#define RuleElement_H

#include "SwUnits.h"

#include <ApplicationTable.h>
#include <Protocols/HACF.h>

class Logger;

class WeekTime;

class RuleElement
{
public:

  struct Condition
  {
    uint32_t senderId;		//## attribute senderId
    uint8_t data[6];		//## attribute data
  };

  struct Action
  {
    uint32_t receiverId;		//## attribute receiverId
    uint8_t length;		//## attribute length
    uint8_t data[5];		//## attribute data
  };

  static const uint8_t DAY_ONLY = 60;
  static const uint8_t NIGHT_ONLY = 61;
  static const uint8_t DAYLY_MASK = 0x40;

  static const uint8_t MAX_CONDITIONS = 64;
  static const uint8_t MAX_ACTIONS = 64;
  static const uint8_t EVERY_STATE = 0xFF;
  static const uint8_t EQUAL_STATE = 0xFF;
  static const uint8_t WILDCARD = 0xFF;

  struct TimeCondition
  {
    uint16_t startTime;		//## attribute startTime
    uint16_t endTime;		//## attribute endTime
  };

  ////    Constructors and destructors    ////

private:

  inline RuleElement()
      : numOfConditions( 0 ),
        numOfActions( 0 ),
        state( 0 ),
        nextState( 0 ),
        startTime( 0 ),
        endTime( 0 )
  {
  }

  ////    Operations    ////

public:

  void getAction( Action* _action, uint8_t index ) const;

  void getCondition( Condition* _condition, uint8_t index ) const;

  uint16_t getLength() const;

  uint8_t getNextState() const;

  uint8_t getNumOfActions() const;

  uint8_t getNumOfConditions() const;

  uint8_t getState() const;

  void getTimeCondition( TimeCondition* _condition ) const;

  bool isActiveForState( uint8_t _state ) const;

  bool isAnyConditionActive( const HACF::ControlFrame& message ) const;

  RuleElement* next() const;

  ////    Attributes    ////

private:

  uint8_t numOfConditions;

  uint8_t numOfActions;

  uint8_t state;

  uint8_t nextState;

  uint16_t startTime;

  uint16_t endTime;

  Condition condition[MAX_CONDITIONS];

  Action action[MAX_ACTIONS];

  static const uint8_t debugLevel;
};

#endif
