/*
 * Rule.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Rule_H
#define Rule_H

#include "SwUnits.h"
#include "RuleElement.h"

#include <Protocols/HACF.h>

class DebugOptions;

class IResponse;

class Rule
{
  ////    Constructors and destructors    ////

public:

  inline Rule()
      : state( 0 ), firstElement( NULL )
  {
  }

  ////    Operations    ////

public:

  bool notifyEvent( const HACF::ControlFrame& message, uint8_t ruleIndex );

  //## operation triggerElement(uint8_t)
  void triggerElement( uint8_t index );

  //## operation triggerElement(RuleElement)
  bool triggerElement( const RuleElement& element );

  inline RuleElement* getFirstElement() const
  {
    return firstElement;
  }

  inline uint8_t getState() const
  {
    return state;
  }

  inline void setFirstElement( RuleElement* p_RuleElement )
  {
    firstElement = p_RuleElement;
  }

  inline void setState( uint8_t p_state )
  {
    state = p_state;
  }

  ////    Attributes    ////

public:

  uint8_t state;		//## attribute state

private:

  static const uint8_t debugLevel;		//## attribute debugLevel

  ////    Relations and components    ////

protected:

  RuleElement* firstElement;		//## link firstElement
};

#endif
