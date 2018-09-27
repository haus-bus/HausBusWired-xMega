/*
 * InternalTemperatureUnit.h
 *
 *  Created on: 22.10.2015
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_InternalTemperatureUnit_H
#define HwUnits_InternalTemperatureUnit_H

#include "BaseSensorUnit.h"

class InternalTemperatureUnit : public BaseSensorUnit
{
   public:

      enum ErrorCodes
      {
         NO_ERROR,
      };

      ////    Constructors and destructors    ////

      InternalTemperatureUnit();

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

   public:

   protected:

      static const uint8_t debugLevel;

   private:



      ////    Relations and components    ////

   protected:

};

inline void* InternalTemperatureUnit::operator new( size_t size )
{
   return allocOnce( size );
}

#endif
