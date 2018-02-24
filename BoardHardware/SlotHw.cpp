/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: AR8
        Configuration   : release
        Model Element	: SlotHw
   //!	Generated Date	: Wed, 6, Aug 2014
        File Path	: AR8/release/Electronics/HwUnitBoards/SlotHw.cpp
 *********************************************************************/

// ## auto_generated
#include "SlotHw.h"
// ## dependency Logger
#include <Traces/Logger.h>
// ## package Electronics::HwUnitBoards

// ## class SlotHw
const uint8_t SlotHw::debugLevel( DEBUG_LEVEL_OFF );

SlotHw::SlotHw() : pwmEnabled( 0 ), digitalOutput0( 0, 0 ), digitalOutput1( 0, 0 )
{
   // #[ operation SlotHw()
   // #]
}

void SlotHw::configure( uint8_t targetType )
{
   // #[ operation configure(uint8_t)
   if ( targetType && ( targetType < MAX_SLOT_TYPES ) )
   {
      type = targetType;
      configureOutput( digitalOutput0, ( type & 1 ) );
      configureOutput( digitalOutput1, ( type & 2 ) );
   }
   else
   {
      type = UNUSED_SLOT;
   }
   // #]
}

void SlotHw::configureOutput( DigitalOutput& output, uint8_t isOutput )
{
   // #[ operation configureOutput(DigitalOutput,uint8_t)
   if ( isOutput )
   {
      output.clear();
      output.getIoPort().setPinsAsOutput( output.getPin() );
   }
   else
   {
      output.getIoPort().configure( output.getPin(), PORT_OPC_PULLUP_gc );
      output.getIoPort().setPinsAsInput( output.getPin() );
   }
   // #]
}

uint16_t SlotHw::getPwmDuty()
{
   // #[ operation getPwmDuty()
   uint8_t pin = digitalOutput0.getPin();

   uint16_t compareValue = 0;
   if ( pin == Pin0 )
   {
      compareValue = timerCounter0->getCaptureA();
   }
   else if ( pin == Pin1 )
   {
      compareValue = timerCounter0->getCaptureB();
   }
   else if ( pin == Pin2 )
   {
      compareValue = timerCounter0->getCaptureC();
   }
   else if ( pin == Pin3 )
   {
      compareValue = timerCounter0->getCaptureD();
   }

   if ( compareValue < timerCounter0->getPeriod() )
   {
      return compareValue / ( timerCounter0->getPeriod() / 1000 );
   }
   return 1000;
   // #]
}

uint8_t SlotHw::hasError()
{
   // #[ operation hasError()
   if ( uint8_t version = isDimmerHw() )
   {
      if ( ( version == V30 ) && digitalOutput1.isSet() )
      {
         return ErrorCode::DEFECT;
      }
      if ( ( version == V31 ) && isOn() && !digitalOutput1.isSet() )
      {
         return ErrorCode::DEFECT;
      }
      if ( timerCounter0->getPeriod() == 0xFFFF )
      {
         return ErrorCode::INVALID_PERIOD;
      }
      if ( !timerCounter0->isRunning() && pwmEnabled )
      {
         return ErrorCode::NO_ZERO_CROSS_DETECTED;
      }
   }
   return ErrorCode::NO_FAILTURE;
   // #]
}

uint16_t SlotHw::isOn()
{
   // #[ operation isOn()
   if ( isDimmerHw() )
   {
      if ( pwmEnabled && timerCounter0->isRunning() )
      {
         return getPwmDuty();
      }
      return digitalOutput0.isSet();
   }
   else if ( isRollerShutterHw() )
   {
      return digitalOutput0.isSet() || digitalOutput1.isSet();
   }
   else if ( isPowerSocketHw() )
   {
      return digitalOutput1.isSet();
   }
   return 0;
   // #]
}

void SlotHw::on( uint16_t value )
{
   // #[ operation on(uint16_t)
   setPwmDuty( value );
   // #]
}

uint8_t SlotHw::setMode( uint8_t mode )
{
   // #[ operation setMode(uint8_t)
   if ( mode == DIMM_L )
   {
      digitalOutput0.setInverted( true );
   }
   else
   {
      digitalOutput0.setInverted( false );
   }
   pwmEnabled = ( mode < SWITCH );
   return ( mode > SWITCH ) ? ErrorCode::INVALID_MODE : ErrorCode::NO_FAILTURE;
   // #]
}

void SlotHw::setPwmDuty( uint16_t duty )
{
   // #[ operation setPwmDuty(uint16_t)
   uint8_t pin = digitalOutput0.getPin();
   if ( pwmEnabled )
   {
      if ( digitalOutput0.isInverted() )
      {
         if ( duty > 1000 )
         {
            duty = 0;
         }
         else
         {
            duty = 1000 - duty;
         }
      }
      uint16_t compareValue = ( timerCounter0->getPeriod() + 500 ) / 1000 * duty;
      DEBUG_M1( compareValue );

      if ( pin == Pin0 )
      {
         timerCounter0->setCompareA( compareValue );
      }
      else if ( pin == Pin1 )
      {
         timerCounter0->setCompareB( compareValue );
      }
      else if ( pin == Pin2 )
      {
         timerCounter0->setCompareC( compareValue );
      }
      else if ( pin == Pin3 )
      {
         timerCounter0->setCompareD( compareValue );
      }
      if ( duty )
      {
         if ( timerCounter0->isRunning() && ( duty < 1000 ) )
         {
            timerCounter0->enableChannels( pin << TC0_CCAEN_bp );
         }
         else
         {
            timerCounter0->disableChannels( pin << TC0_CCAEN_bp );
            digitalOutput0.set();
         }
      }
      else
      {
         timerCounter0->disableChannels( pin << TC0_CCAEN_bp );
         digitalOutput0.clear();
      }
   }
   else
   {
      timerCounter0->disableChannels( pin << TC0_CCAEN_bp );
      if ( duty )
      {
         digitalOutput0.set();
      }
      else
      {
         digitalOutput0.clear();
      }
   }
   // #]
}

void SlotHw::stop()
{
   // #[ operation stop()
   digitalOutput0.clear();
   digitalOutput1.clear();
   // #]
}

uint8_t SlotHw::suspend( uint8_t mode )
{
   // #[ operation suspend(uint8_t)
   if ( isDimmerHw() == V31 )
   {
      if ( mode == ENABLE )
      {
         digitalOutput1.configOutput();
         digitalOutput1.clear();
      }
      else if ( mode == DISABLE )
      {
         digitalOutput1.configOutput();
         digitalOutput1.set();
      }
      else
      {
         digitalOutput1.configInput();
      }
      _delay_us( 100 );
   }
   return true;

   // #]
}

DigitalOutput* SlotHw::getDigitalOutput0() const
{
   return (DigitalOutput*) &digitalOutput0;
}

DigitalOutput* SlotHw::getDigitalOutput1() const
{
   return (DigitalOutput*) &digitalOutput1;
}

/*********************************************************************
        File Path	: AR8/release/Electronics/HwUnitBoards/SlotHw.cpp
*********************************************************************/
