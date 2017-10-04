/*
 * HBW-SD6-Booter.cpp
 *
 * Created: 30.09.2017 02:31:44
 *  Author: Viktor Pankraz
 */


#define HMW_DEVICETYPE 0xB1

#define HARDWARE_VERSION 0x01
#define FIRMWARE_VERSION 0x0101

#include "HBW-SD6-Booter.h"
#include "HBWired/HBWired.h"

#include <Peripherals/InterruptController.h>
#include <Peripherals/WatchDog.h>
#include <Peripherals/Oscillator.h>
#include <Peripherals/Clock.h>
#include <Peripherals/Flash.h>

#include <Time/SystemTime.h>
#include <Elements/SerialStream.h>
#include <HBWired/HmwMessage.h>


const ModuleId moduleId =
{
   "$MOD$ HBW-SD6-B",
   0,
   FIRMWARE_VERSION >> 8,
   FIRMWARE_VERSION & 0xFF,
   HMW_DEVICETYPE,
   0
};

static HBWDevice::BasicConfig& config = *reinterpret_cast<HBWDevice::BasicConfig*>( MAPPED_EEPROM_START );

#ifdef DEBUG
void putc( char c )
{
   Usart::instance<PortC, 1>().write( c );
}
#endif


int main( void )
{
   SystemTime::init();
   Eeprom::MemoryMapped::enable();

   // Initialize interfaces
   SerialStream rs485Stream( &Usart::instance<PortE, 0>(), PortPin( PortE, 2 ), PortPin( PortE, 3 ) );
   rs485Stream.init<19200, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc, USART_CHSIZE_8BIT_gc, true, false>();

    #ifdef DEBUG
   SerialStream debugStream( &Usart::instance<PortC, 1>(), PortPin( PortC, 6 ), PortPin( PortC, 7 ) );
   debugStream.init<115200>();
   Logger::instance().setStream( putc );
    #endif


   // Authorize interrupts
   InterruptController::enableAllInterruptLevel();
   GlobalInterrupt::enable();

   HmwMessage receivedMessage, sentMessage;
   Timestamp lastReceivedTime = Timestamp();
   uint8_t data;
   while ( 1 )
   {
      if ( rs485Stream.available() && ( rs485Stream.read( data ) == Stream::SUCCESS ) )
      {
         lastReceivedTime = Timestamp();
         if ( receivedMessage.nextByteReceived( data ) )
         {
            if ( receivedMessage.isForMe( changeEndianness( config.ownAdress ) ) && !receivedMessage.isDiscovery() )
            {
               if ( receivedMessage.isCommand( HmwMessage::GET_FW_VERSION ) )
               {
                  receivedMessage.prepareToSend();
                  while ( receivedMessage.getNextByteToSend( data ) )
                  {
                     if ( rs485Stream.write( data ) != Stream::SUCCESS )
                     {
                        break;
                     }
                  }
               }
               else if ( !receivedMessage.isBroadcast() )
               {
                  receivedMessage.changeIntoACK();
                  receivedMessage.prepareToSend();
                  while ( receivedMessage.getNextByteToSend( data ) )
                  {
                     if ( rs485Stream.write( data ) != Stream::SUCCESS )
                     {
                        break;
                     }
                  }
               }
            }
         }
      }
      WatchDog::reset();
   }
   return 0;
}


static void
__attribute__( ( section( ".init3" ), naked, used ) )
lowLevelInit( void )
{
        #ifdef EIND
   __asm volatile ( "ldi r24,pm_hh8(__trampolines_start)\n\t"
                    "out %i0,r24" ::"n" ( &EIND ) : "r24", "memory" );
        #endif
        #ifdef DEBUG
   WatchDog::disable();
        #else
   WatchDog::enable( WatchDog::_4S );
        #endif
   InterruptController::selectBootInterruptSection();

   Clock::configPrescalers( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );

   // Enable internal 32 MHz and 32kHz ring oscillator and wait until they are stable.
   Oscillator::enable( OSC_RC32MEN_bm | OSC_RC32KEN_bm );
   Oscillator::waitUntilOscillatorIsReady( OSC_RC32MEN_bm | OSC_RC32KEN_bm );

   // Set the 32 MHz ring oscillator as the main clock source.
   Clock::selectMainClockSource( CLK_SCLKSEL_RC32M_gc );
}