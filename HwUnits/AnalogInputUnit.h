/*
 * AnalogInputUnit.h
 *
 *  Created on: 22.10.2015
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_AnalogInputUnit_H
#define HwUnits_AnalogInputUnit_H

#include "HwUnits.h"
#include "BaseSensorUnit.h"
#include <AnalogInput.h>

class AnalogInputUnit : public BaseSensorUnit
{
   public:

      enum ErrorCodes
      {
         NO_ERROR,
      };

      ////    Constructors and destructors    ////

      AnalogInputUnit( PortPin portPin );

      ////    Operations    ////

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size );

   private:

      void run();

      ////    Additional operations    ////

   public:


   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

   private:

      ////    Attributes    ////

      AnalogInput analogInput;

   public:

   protected:

      static const uint8_t debugLevel;

   private:



      ////    Relations and components    ////

   protected:

};

inline void* AnalogInputUnit::operator new( size_t size )
{
   return allocOnce( size );
}

#endif
