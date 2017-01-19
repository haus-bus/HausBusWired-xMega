/*
 * RuleEngine.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef RuleEngine_H
#define RuleEngine_H

#include "SwUnits.h"
#include "Rule.h"

#include <Protocols/HACF.h>

class FlashString;

class RuleEngine
{
  ////    Operations    ////

public:

#ifdef _DEBUG_
  static FlashString* getId();
#endif

  static void notifyEvent( const HACF::ControlFrame& message );

  inline static void disable()
  {
    setNumOfRules( 0 );
  }

  inline static uint8_t getNumOfRules()
  {
    return numOfRules;
  }

  inline static Rule* getRules()
  {
    return rules;
  }

  inline static void setNumOfRules( uint8_t p_numOfRules )
  {
    numOfRules = p_numOfRules;
  }

  inline static void setRules( Rule* p_rules )
  {
    rules = p_rules;
  }

  ////    Attributes    ////

public:

  static uint8_t numOfRules;

  static Rule* rules;

private:

  static const uint8_t debugLevel;
};

#endif
