/*
 * UltrasonicDistanceMeter.cpp
 *
 *  Created on: 22.07.2016
 *      Author: viktor.pankraz
 */

#include <UltrasonicDistanceMeter.h>
#include <ErrorMessage.h>

const uint8_t UltrasonicDistanceMeter::debugLevel( DEBUG_LEVEL_OFF );

UltrasonicDistanceMeter::UltrasonicDistanceMeter( DigitalOutput _triggerPin,
                                                  DigitalInput _measurePin ) :
   triggerPin( _triggerPin ), echoPin( _measurePin )
{

   Object::setId( ( ClassId::USD_METER << 8 )
                  | ( ( triggerPin.getPortNumber() + 1 ) << 4 )
                  | ( triggerPin.getPinNumber() + 1 ) );
}

bool UltrasonicDistanceMeter::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      run();
   }
   else if ( event.isEvMessage() )
   {
      return handleRequest( event.isEvMessage()->getMessage() );
   }

   return false;
}

void UltrasonicDistanceMeter::run()
{
   if ( inStartUp() )
   {
      setConfiguration( ConfigurationManager::getConfiguration<EepromConfiguration>( id ) );
      if ( !configuration )
      {
         terminate();
         ErrorMessage::notifyOutOfMemory( id );
         return;
      }
      SET_STATE_L1( RUNNING );
   }

   // The PING is triggered by a HIGH pulse of 2 or more microseconds.
   // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
   triggerPin.clear();
   delayUs( 2 );
   triggerPin.set();
   delayUs( 10 );
   triggerPin.clear();

   // The same pin is used to read the signal from the PING: a HIGH
   // pulse whose duration is the time (in microseconds) from the sending
   // of the ping to the reception of its echo off of an object.
   uint16_t duration = 100; // todo echoPin.getPulseWidth( PortPin::HIGH, 10000 );

   // convert the time into a distance
   uint8_t cm = microsecondsToCentimeters( duration );

   DEBUG_H2( FSTR( ".value: 0x" ), cm );

   Status status;
   status.value = cm;
   status.centiValue = 0;
   notifyNewValue( status );
   setSleepTime( SystemTime::S );
}
