/*
 * Rule.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "Rule.h"
#include "DebugOptions.h"
#include "IResponse.h"

const uint8_t Rule::debugLevel( DEBUG_LEVEL_OFF );

bool Rule::notifyEvent( const HACF::ControlFrame& message, uint8_t ruleIndex )
{
   RuleElement* element = firstElement;
   uint8_t idx = 0;
   do
   {
      DEBUG_M4( idx, FSTR( " state: " ), state, ( element->isActiveForState( state ) ? FSTR( " activ" ) : FSTR( " inactiv" ) ) );
      if ( element->isActiveForState( state ) )
      {
         DEBUG_M4( idx, FSTR( " conditions: " ), element->getNumOfConditions(), ( element->isAnyConditionActive( message ) ? FSTR( " activ" ) : FSTR( " inactiv" ) ) );
         if ( element->getNumOfConditions()
            && element->isAnyConditionActive( message ) )
         {
            if ( DebugOptions::notifyTriggeredRule() )
            {
               IResponse triggerMsg( HACF::SYSTEM_ID );
               triggerMsg.controlFrame.setDataLength( 3 );
               triggerMsg.controlFrame.data[0] = 136;
               triggerMsg.controlFrame.data[1] = ruleIndex;
               triggerMsg.controlFrame.data[2] = idx;
               triggerMsg.queue();
            }

            if ( triggerElement( *element ) )
            {
               break;
            }
         }
      }
      element = element->next();
      idx++;
   }
   while ( element );

   return true;
}

void Rule::triggerElement( uint8_t index )
{
   RuleElement* rule = firstElement;
   while ( rule && index-- )
   {
      rule = rule->next();
   }
   if ( rule )
   {
      triggerElement( *rule );
   }
}

bool Rule::triggerElement( const RuleElement& element )
{
   RuleElement::Action action;

   uint8_t index = element.getNumOfActions();
   while ( index-- )
   {
      DEBUG_M2( FSTR( "a:" ), index );
      IResponse actionMsg( HACF::SYSTEM_ID );
      element.getAction( &action, index );

      actionMsg.controlFrame.receiverId.setId( action.receiverId );
      actionMsg.controlFrame.setDataLength( action.length );
      while ( action.length-- )
      {
         DEBUG_L2( ' ', action.data[action.length] );
         actionMsg.controlFrame.getData()[action.length]
            = action.data[action.length];
      }
      actionMsg.queue();
   }
   uint8_t nextState = element.getNextState();
   if ( nextState != RuleElement::EQUAL_STATE )
   {
      DEBUG_M4( FSTR( "state: " ), state, FSTR( "->" ), nextState );
      setState( nextState );
      return true;
   }

   return false;
}
