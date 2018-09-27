/*
 * UltrasonicDistanceMeter.h
 *
 *  Created on: 22.07.2016
 *      Author: viktor.pankraz
 */

#ifndef ULTRASONICDISTANCEMETER_H_
#define ULTRASONICDISTANCEMETER_H_

#include "BaseSensorUnit.h"
#include <DigitalOutput.h>

/*
 *
 */
class UltrasonicDistanceMeter : public BaseSensorUnit
{
   public:

      UltrasonicDistanceMeter( DigitalOutput _triggerPin, DigitalInput _measurePin );


      virtual bool notifyEvent( const Event& event );

      inline uint16_t microsecondsToCentimeters( uint16_t microseconds )
      {
         // The speed of sound is 340 m/s or 29 microseconds per centimeter.
         // The ping travels out and back, so to find the distance of the
         // object we take half of the distance traveled.
         return microseconds / 29 / 2;
      }

      inline void* operator new( size_t size )
      {
         return allocOnce( size );
      }

   protected:

   private:

      void run();

      ////    Additional operations    ////


   protected:

      static const uint8_t debugLevel;

      DigitalOutput triggerPin;

      DigitalInput echoPin;

   private:

};



#endif /* ULTRASONICDISTANCEMETER_H_ */
