/*
 * PersistentRules.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "PersistentRules.h"
#include "RuleEngine.h"

#include <Traces/Logger.h>

const uint8_t PersistentRules::debugLevel( DEBUG_LEVEL_OFF );

bool PersistentRules::init()
{
   DEBUG_M1( FSTR( "configureRules" ) );

   uint8_t _numOfRules = getNumOfRules();
   if ( _numOfRules && ( _numOfRules != 0xFF ) )
   {
      delete RuleEngine::rules;

      DEBUG_M2( FSTR( "num: " ), _numOfRules );
      Rule* rules = new Rule[_numOfRules];
      if ( rules )
      {
         RuleEngine::setNumOfRules( _numOfRules );
         RuleEngine::setRules( rules );

         const RuleElement* rule = next();
         uint8_t i;
         for ( i = 0; ( i < _numOfRules ) && rule; i++ )
         {
            DEBUG_M3( i, '\t', (uintptr_t)rule );
            rules[i].setFirstElement( const_cast<RuleElement*>( rule ) );
            rule = next( rule );
         }
         if ( ( i != _numOfRules ) || rule )
         {
            RuleEngine::setNumOfRules( i );
            return false;
         }
      }
      return true;
   }
   RuleEngine::setNumOfRules( 0 );

   return _numOfRules ? false : true;
}

const RuleElement* PersistentRules::next( const RuleElement* last ) const
{
   if ( last )
   {
      DEBUG_L4( "\tc: ", last->getNumOfConditions(), " a: ", last->getNumOfActions() );
      RuleElement* nextElement = last->next();
      while ( nextElement )
      {
         last = nextElement;
         DEBUG_M2( '\t', (uintptr_t)last ); DEBUG_L4( "\tc: ", last->getNumOfConditions(), " a: ", last->getNumOfActions() );
         nextElement = last->next();
      }
      last = reinterpret_cast<RuleElement*>( ( (uintptr_t) last ) + 1 );
      if ( last->getNumOfConditions() )
      {
         return last;
      }
      return 0;
   }
   return &firstRuleElement;
   // #]
}
