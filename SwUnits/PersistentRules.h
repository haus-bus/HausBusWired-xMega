/*
 * PersistentRules.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef PersistentRules_H
#define PersistentRules_H

#include "SwUnits.h"
#include "RuleElement.h"

#include <ApplicationTable.h>

class Logger;

class RuleEngine;

struct PersistentRules
{
   ////    Constructors and destructors    ////

   private:

      ////    Operations    ////


   public:

      bool init();

      const RuleElement* next( const RuleElement* last = 0 ) const;

      inline static PersistentRules& instance()
      {
         return *reinterpret_cast<PersistentRules*>( 0 );
      }

      inline uint8_t getNumOfRules() const
      {
         return ApplicationTable::read( reinterpret_cast<uintptr_t>( &numOfRules ) );
      }

      ////    Attributes    ////

   private:

      const uint8_t numOfRules = 0;

      const RuleElement firstRuleElement;

      static const uint8_t debugLevel;
};

#endif

