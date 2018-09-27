/*
 * BatteryManager.cpp
 *
 *  Created on: 11.04.2016
 *      Author: viktor.pankraz
 */

#include <BatteryManager.h>
#include <SwFramework.h>
#include <ErrorMessage.h>

const uint8_t BatteryManager::debugLevel( DEBUG_LEVEL_OFF );

BatteryManager::BatteryManager( DigitalOutput _charger,
                                DigitalInput _powerSupply,
                                AnalogInput _batteryGndVoltage ) :
   batteryValue( 0 ), charger( _charger ), powerSupply( _powerSupply ),
   batteryGndVoltage( _batteryGndVoltage )
{
   setId( ( Object::ClassId::BATTERY_MANAGER << 8 ) | 1 );
   charger.configOutput();
   disableCharger();
   setConfiguration( ConfigurationManager::getConfiguration<EepromConfiguration>( id ) );
   if ( configuration )
   {
// TODO
   }
   else
   {
      terminate();
      ErrorMessage( getId(), ErrorMessage::CONFIGURATION_OUT_OF_MEMORY );
   }
}

void BatteryManager::run()
{
   disableCharger();

   if ( inStartUp() )
   {
      SET_STATE_L1( RUNNING );
   }

   if ( inRunning() )
   {
      if ( hasBatteryStatusChanged() )
      {
         sendErrorStatus();
      }
      if ( getCurrentSubState() == NO_ERROR )
      {
         enableCharger();
      }
      notifyCurrentBatteryValue();
   }
   setSleepTime( SystemTime::S );
}

void BatteryManager::sendErrorStatus()
{
   IResponse event( getId() );
   event.setErrorCode( state.sub );
   event.queue();
   DEBUG_H2( FSTR( ".sendErrorStatus " ), state.sub );
}

bool BatteryManager::notifyEvent( const Event& event )
{
   bool consumed = true;

   if ( event.isEvWakeup() )
   {
      run();
   }
   else if ( event.isEvMessage() )
   {
      return handleRequest( event.isEvMessage()->getMessage() );
   }
   else
   {
      consumed = false;
   }

   return consumed;
}

BatteryManager::SubStates BatteryManager::getBatteryStatus()
{
   batteryValue = 0;
   // sample some values
   uint32_t currentValue = 0;
   uint8_t i = 8;
   while ( i-- )
   {
      currentValue += batteryGndVoltage.getValue();
   }
   currentValue /= 8;

   if ( currentValue > 0xF00 )
   {
      return BATTERY_REVERSE_POLARITY;
   }
   if ( currentValue > 0x600 )
   {
      return BATTERY_NOT_CONNECTED;
   }
   if ( !currentValue )
   {
      return POWER_SUPPLY_NOT_CONNECTED;
   }
   batteryValue = currentValue;
   return NO_ERROR;
}

bool BatteryManager::hasBatteryStatusChanged()
{
   SubStates batteryStatus = getBatteryStatus();
   bool result = false;

   if ( batteryStatus != NO_ERROR )
   {
      result = ( batteryStatus != getCurrentSubState() );
      SET_STATE_L2( batteryStatus );
   }
   else if ( isPowerSupplyOk() )
   {
      result = ( NO_ERROR != getCurrentSubState() );
      SET_STATE_L2( NO_ERROR );
   }
   else
   {
      result = ( POWER_SUPPLY_NOK != getCurrentSubState() );
      SET_STATE_L2( POWER_SUPPLY_NOK );
   }
   return result;
}


void BatteryManager::notifyCurrentBatteryValue()
{
   static uint8_t counter = 60;
   if ( ++counter >= 60 )
   {
      counter = 0;
      Status status;
      uint16_t tmp = 13700 - batteryValue;

      if ( state.sub == BATTERY_REVERSE_POLARITY )
      {
         tmp = 0xFFFF;
      }

      if ( state.sub == BATTERY_NOT_CONNECTED )
      {
         tmp = 0;
      }

      status.value = tmp / 1000;
      status.centiValue = ( tmp % 1000 ) / 10;
      notifyNewValue( status );
   }
}
