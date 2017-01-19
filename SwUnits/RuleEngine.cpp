/*
 * RuleEngine.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "RuleEngine.h"

#include <FlashString.h>

uint8_t RuleEngine::numOfRules( 0 );

Rule* RuleEngine::rules( 0 );

const uint8_t RuleEngine::debugLevel( DEBUG_LEVEL_OFF );

#ifdef _DEBUG_
FlashString* RuleEngine::getId()
{
  return FSTR("RuleEngine ");
}
#endif

void RuleEngine::notifyEvent( const HACF::ControlFrame& message )
{
  uint8_t idx = numOfRules;
  while ( idx-- )
  {
    DEBUG_H2( FSTR("rule "), idx );
    rules[idx].notifyEvent( message, idx );
  }
}
