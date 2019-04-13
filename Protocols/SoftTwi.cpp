/*
 * SoftTwi.cpp
 *
 *  Created on: 28.12.2016
 *      Author: viktor.pankraz
 */

#include <Basics.h>
#include <Protocols/HBCP.h>
#include <Peripherals/IoPort.h>
#include <util/delay.h>
#include <EventPkg/EventPkg.h>
#include <Protocols/SoftTwi.h>


// #define TEST_AR8


#define TRACE_INIT( port, pins )     port.DIRSET = pins; port.OUTCLR = pins
#define TRACE_PUT( port, pins )      port.OUT = pins
#define TRACE_SET( port, pins )      port.OUTSET = pins
#define TRACE_CLEAR( port, pins )    port.OUTCLR = pins
#define TRACE_TOGGLE( port, pins )   port.OUTTGL = pins

#ifdef TEST_SD6
#define TRACE_TWI_INIT               TRACE_INIT( PORTB, Pin0 | Pin1 | Pin2 | Pin3 ); \
   TRACE_INIT( PORTC, Pin6 | Pin7 ); \
   TRACE_INIT( PORTD, Pin0 | Pin1 | Pin2 | Pin3 | Pin4 | Pin5 | Pin6 )

#define TRACE_TWI_STATE_IDLE         TRACE_CLEAR( PORTB, Pin0 | Pin1 )
#define TRACE_TWI_STATE_WRITING      TRACE_SET( PORTB, Pin0 | Pin1 )
#define TRACE_TWI_STATE_READING      TRACE_SET( PORTB, Pin0 ) TRACE_CLEAR( PORTB, Pin1 )

#define TRACE_TWI_WRITE_BIT_START    TRACE_SET( PORTD, Pin0 )
#define TRACE_TWI_WRITE_BIT_END      TRACE_CLEAR( PORTD, Pin0 )
#define TRACE_TWI_READ_BIT_START     TRACE_SET( PORTD, Pin1 )
#define TRACE_TWI_READ_BIT_END       TRACE_CLEAR( PORTD, Pin1 )

#define TRACE_TWI_SCL_HOLD           TRACE_CLEAR( PORTD, Pin4 )
#define TRACE_TWI_SCL_RELEASE        TRACE_SET( PORTD, Pin4 )

#define TRACE_TWI_INT1_START         TRACE_SET( PORTD, Pin5 )
#define TRACE_TWI_INT1_END           TRACE_CLEAR( PORTD, Pin5 )


#define TRACE_TWI_WRITE_ACK_START    TRACE_SET( PORTC, Pin6 )
#define TRACE_TWI_WRITE_ACK_END      TRACE_CLEAR( PORTC, Pin6 )
#define TRACE_TWI_READ_ACK_START     TRACE_SET( PORTC, Pin7 )
#define TRACE_TWI_READ_ACK_END       TRACE_CLEAR( PORTC, Pin7 )
#else
#ifdef TEST_AR8

#define TRACE_TWI_INIT               TRACE_INIT( PORTF, AllPins )
#define TRACE_TWI_STATE_IDLE         TRACE_CLEAR( PORTF, Pin0 | Pin1 )
#define TRACE_TWI_STATE_WRITING      TRACE_SET( PORTF, Pin0 | Pin1 )
#define TRACE_TWI_STATE_READING      TRACE_SET( PORTF, Pin0 ); TRACE_CLEAR( PORTB, Pin1 )

#define TRACE_TWI_WRITE_BIT_START    TRACE_SET( PORTF, Pin2 )
#define TRACE_TWI_WRITE_BIT_END      TRACE_CLEAR( PORTF, Pin2 )
#define TRACE_TWI_READ_BIT_START     TRACE_SET( PORTF, Pin3 )
#define TRACE_TWI_READ_BIT_END       TRACE_CLEAR( PORTF, Pin3 )

#define TRACE_TWI_WRITE_ACK_START    TRACE_SET( PORTF, Pin4 )
#define TRACE_TWI_WRITE_ACK_END      TRACE_CLEAR( PORTF, Pin4 )
#define TRACE_TWI_READ_ACK_START     TRACE_SET( PORTF, Pin5 )
#define TRACE_TWI_READ_ACK_END       TRACE_CLEAR( PORTF, Pin5 )

#define TRACE_TWI_SCL_HOLD           TRACE_CLEAR( PORTF, Pin6 )
#define TRACE_TWI_SCL_RELEASE        TRACE_SET( PORTF, Pin6 )

#define TRACE_TWI_INT1_START         TRACE_SET( PORTF, Pin7 )
#define TRACE_TWI_INT1_END           TRACE_CLEAR( PORTF, Pin7 )

#else

#define TRACE_TWI_INIT
#define TRACE_TWI_STATE_IDLE
#define TRACE_TWI_STATE_WRITING
#define TRACE_TWI_STATE_READING

#define TRACE_TWI_WRITE_BIT_START
#define TRACE_TWI_WRITE_BIT_END
#define TRACE_TWI_READ_BIT_START
#define TRACE_TWI_READ_BIT_END

#define TRACE_TWI_WRITE_ACK_START
#define TRACE_TWI_WRITE_ACK_END
#define TRACE_TWI_READ_ACK_START
#define TRACE_TWI_READ_ACK_END

#define TRACE_TWI_SCL_HOLD
#define TRACE_TWI_SCL_RELEASE

#define TRACE_TWI_INT1_START
#define TRACE_TWI_INT1_END

#endif
#endif



#define TWI_PORT              PORTE
#define TWI_PER               TWIE
#define SCL_PIN               Pin1
#define SDA_PIN               Pin0
#define STATE_HIGH            true
#define STATE_LOW             false
#define TIMEOUT_COUNT         50000
#define DEBOUNCE_COUNT_HIGH   20
#define DEBOUNCE_COUNT_LOW    10
#define DEBOUNCE_COUNT_READ   4
#define START_STOP_DELAY      50
#define MAX_STOP_RETRIES      5

#define ENABLE_SCL_INT        TWI_PORT.INTFLAGS = PORT_INT0IF_bm; \
   TWI_PORT.INTCTRL = ( TWI_PORT.INTCTRL & ~PORT_INT0LVL_gm ) | PORT_INT0LVL_HI_gc;
#define DISABLE_SCL_INT       TWI_PORT.INTCTRL &= ~PORT_INT0LVL_gm;

volatile bool isMaster = false;
volatile bool hasTimeout = false;
volatile IStream::State streamState = IStream::IDLE;

const uint8_t debugLevel( DEBUG_LEVEL_OFF );

template<uint8_t debounceCount, uint16_t waitCount, uint8_t pin, bool state, bool stopOnIdle>
uint16_t io_waitForDebouncedPinState()
{
   uint8_t debounce = debounceCount;
   uint16_t wait = waitCount;

   if ( waitCount <= debounceCount )
   {
      // it does not make sense that waitcount should be lower than the debounceCount
      wait = debounceCount + 1;
   }

   while ( wait )
   {
      if ( ( ( TWI_PORT.IN & pin ) && state ) || ( !( TWI_PORT.IN & pin ) && !state ) )
      {
         if ( debounce == 0 )
         {
            break;
         }
         debounce--;
      }
      else
      {
         debounce = debounceCount;
      }
      wait--;

      if ( wait <= debounce )
      {
         // waitloop can be shorten in this case
         return 0;
      }
      if ( stopOnIdle && ( streamState == IStream::IDLE ) )
      {
         return 0xFFFF;
      }
   }
   return wait;
}

template<uint8_t pin, bool state, bool drive, bool stopOnIdle>
bool handleTwiPin()
{
   if ( state )
   {
      TWI_PORT.DIRCLR = pin;
      if ( pin == SCL_PIN )
      {
         TRACE_TWI_SCL_RELEASE;
      }
   }
   else if ( drive )
   {
      TWI_PORT.DIRSET = pin;
   }
   uint16_t remaining = io_waitForDebouncedPinState<
      ( state ?
        ( drive ? DEBOUNCE_COUNT_HIGH : DEBOUNCE_COUNT_READ ) :
        ( drive ? DEBOUNCE_COUNT_LOW : DEBOUNCE_COUNT_READ ) ), TIMEOUT_COUNT,
      pin, state, stopOnIdle>();
   if ( remaining )
   {
      return state;
   }
   hasTimeout = true;
   return !state;
}

template<uint8_t pin, uint8_t debounceCount>
bool getDebouncedPinState()
{
   uint8_t debounce = debounceCount;
   uint8_t lastState = TWI_PORT.IN & pin;

   while ( debounce )
   {
      if ( ( TWI_PORT.IN & pin ) == lastState )
      {
         debounce--;
      }
      else
      {
         lastState = TWI_PORT.IN & pin;
         debounce = debounceCount;
      }
   }
   return lastState;
}

template<uint8_t pin>
bool readTwiPin()
{
   return getDebouncedPinState<pin, DEBOUNCE_COUNT_READ>();
}

void enableSclInt()
{
   // clear pending Int0
   TWI_PORT.INTFLAGS = PORT_INT0IF_bm;

   // enable Int0
   TWI_PORT.INTCTRL = ( TWI_PORT.INTCTRL & ~PORT_INT0LVL_gm )
                      | PORT_INT0LVL_HI_gc;
}


IStream::Status startTx()
{
   isMaster = false;

   if ( streamState != IStream::IDLE )
   {
      return IStream::LOCKED;
   }

   if ( handleTwiPin<SCL_PIN, STATE_HIGH, true, false>() )
   {
      handleTwiPin<SDA_PIN, STATE_LOW, true, false>();
      _delay_us( START_STOP_DELAY );
      if ( handleTwiPin<SCL_PIN, STATE_HIGH, true, false>() == STATE_HIGH )
      {
         // no error
         isMaster = true;
         return IStream::SUCCESS;
      }
   }
   // arbitration lost
   return IStream::ABORTED;
}

IStream::Status sendReceiveByte( uint8_t& data )
{
   uint8_t mask = 0x80;

   while ( mask )
   {
      bool lastBitSent = STATE_HIGH;

      if ( hasTimeout )
      {
         return IStream::TIMEOUT;
      }

      if ( isMaster )
      {
         // drive falling edge on SCL
         isMaster = ( STATE_LOW == handleTwiPin<SCL_PIN, STATE_LOW, true, false>() );
      }
      else
      {
         // wait for falling edge on SCL or STOP condition
         handleTwiPin<SCL_PIN, STATE_LOW, false, true>();
         TRACE_TWI_SCL_RELEASE;
      }

      if ( streamState == IStream::IDLE )
      {
         return IStream::STOPPED;
      }

      if ( isMaster )
      {
         if ( data & mask )
         {
            // set high bit on the SDA line
            TRACE_TWI_WRITE_BIT_START;
            isMaster = ( STATE_HIGH == handleTwiPin<SDA_PIN, STATE_HIGH, true, false>() );
            TRACE_TWI_WRITE_BIT_END;

            lastBitSent = STATE_HIGH;
         }
         else
         {
            // set low bit on the SDA line
            TRACE_TWI_WRITE_BIT_START;
            isMaster = ( STATE_LOW == handleTwiPin<SDA_PIN, STATE_LOW, true, false>() );
            TRACE_TWI_WRITE_BIT_END;

            lastBitSent = STATE_LOW;
         }
      }
      if ( isMaster )
      {
         // drive rising edge on SCL
         isMaster = ( STATE_HIGH == handleTwiPin<SCL_PIN, STATE_HIGH, true, false>() );
      }
      else
      {
         // wait for rising edge on SCL
         handleTwiPin<SCL_PIN, STATE_HIGH, false, false>();
         TRACE_TWI_SCL_RELEASE;
      }
      ENABLE_SCL_INT;
      TRACE_TWI_READ_BIT_START;
      bool sdaState = readTwiPin<SDA_PIN>();
      TRACE_TWI_READ_BIT_END;

      if ( isMaster && ( sdaState != lastBitSent ) )
      {
         isMaster = false;
      }
      if ( sdaState )
      {
         data |= mask;
      }
      else
      {
         data &= ~mask;
      }
      mask >>= 1;
   }

   // The 9th clock (ACK Phase)
   if ( isMaster )
   {
      // drive falling edge on SCL
      handleTwiPin<SCL_PIN, STATE_LOW, true, false>();
      // release SDA line without checking the state because the state will be driven by slaves
      TWI_PORT.DIRCLR = SDA_PIN;
      _delay_us( 5 );
      // drive rising edge on SCL
      isMaster = ( STATE_HIGH == handleTwiPin<SCL_PIN, STATE_HIGH, true, false>() );
   }
   else
   {
      handleTwiPin<SCL_PIN, STATE_LOW, false, false>();

      TRACE_TWI_WRITE_ACK_START;
      handleTwiPin<SDA_PIN, STATE_LOW, true, false>();
      TRACE_TWI_WRITE_ACK_END;

      handleTwiPin<SCL_PIN, STATE_HIGH, false, false>();
   }
   TRACE_TWI_READ_ACK_START;
   bool nack = readTwiPin<SDA_PIN>();
   TRACE_TWI_READ_ACK_END;

   if ( isMaster )
   {
      handleTwiPin<SCL_PIN, STATE_LOW, true, false>();
   }
   else
   {
      handleTwiPin<SCL_PIN, STATE_LOW, false, false>();
      TWI_PORT.DIRCLR = SDA_PIN;
   }
   if ( nack )
   {
      return IStream::NO_ACK;
   }
   return IStream::SUCCESS;
}

void stopTx()
{
   int8_t stopRetries = MAX_STOP_RETRIES;
   handleTwiPin<SDA_PIN, STATE_LOW, true, true>();
   while ( stopRetries-- && ( STATE_HIGH != handleTwiPin<SCL_PIN, STATE_HIGH, true, true>() ) )
   {
   }
   _delay_us( START_STOP_DELAY );
   handleTwiPin<SDA_PIN, STATE_HIGH, true, true>();
   isMaster = false;
}

void init()
{
   TWI_PER.CTRL = 0;
   TWI_PER.MASTER.CTRLA = 0;
   TWI_PER.SLAVE.CTRLA = 0;

   TRACE_TWI_INIT;

   ( (IoPort*)&TWI_PORT )->configure( SCL_PIN, PORT_OPC_PULLUP_gc, false, PORT_ISC_FALLING_gc, true );
   ( (IoPort*)&TWI_PORT )->configure( SDA_PIN, PORT_OPC_PULLUP_gc, false, PORT_ISC_BOTHEDGES_gc, true );
   ( (IoPort*)&TWI_PORT )->enableInterrupt0Source( SCL_PIN );
   ( (IoPort*)&TWI_PORT )->enableInterrupt1Source( SDA_PIN );
   ( (IoPort*)&TWI_PORT )->enableInterrupt1( PORT_INT1LVL_HI_gc );
}

IStream::Status SoftTwi::genericCommand( IoStream::Command command,
                                         void* buffer )
{
   if ( command == IoStream::INIT )
   {
      // IoStream::CommandINIT* cmd = (IoStream::CommandINIT*) buffer;
      init();
      return SUCCESS;
   }
   return ABORTED;
}

IStream::Status SoftTwi::read( void* pData, uint16_t length, EventDrivenUnit* user )
{
   uint16_t bytesTransferred = 0;
   IStream::Status status = IStream::NO_DATA;
   hasTimeout = false;

   bool wasMaster = isMaster;

   while ( streamState != IStream::IDLE )
   {
      uint8_t data = 0xFF;
      status = sendReceiveByte( length ? ( (uint8_t*)pData )[bytesTransferred] : data );

      if ( ( wasMaster != isMaster ) && ( streamState == IStream::WRITING ) )
      {
         // increase buffer while switching to slave mode
         length = HBCP::MAX_BUFFER_SIZE - bytesTransferred;
         streamState = IStream::READING;
         status = IStream::ARB_LOST;
      }

      if ( status == IStream::TIMEOUT )
      {
         if ( !wasMaster )
         {
            // a timeout in slave mode should set the streamState into IDLE
            // to wait for the next real start condition
            CriticalSection doNotInterrupt;
            streamState = IStream::IDLE;
         }
         break;
      }

      if ( status == IStream::NO_ACK )
      {
         if ( bytesTransferred == 0 )
         {
            status = IStream::NO_RECEIVER;
         }
         break;
      }

      if ( status == IStream::STOPPED )
      {
         IStream::TransferDescriptor td;
         td.bytesTransferred = bytesTransferred;
         td.pData = (uint8_t*)pData;
         evData( user, &td ).send();
         break;
      }

      if ( length )
      {
         bytesTransferred++;
         length--;
         if ( ( length == 0 ) && isMaster )
         {
            status = IStream::SUCCESS;
            break;
         }
      }
      else
      {
         status = IStream::INVALID_BUFFER;
      }
   }

   CriticalSection doNotInterrupt;
   if ( ( status == IStream::NO_DATA ) && ( streamState != IStream::READING ) )
   {
      // release pins on error or in IDLE
      TWI_PORT.DIRCLR = SDA_PIN | SCL_PIN;

      // disable Int0
      DISABLE_SCL_INT;
   }

   return status;
}


IStream::Status SoftTwi::write( void* pData, uint16_t length, EventDrivenUnit* user )
{
   // allocate the bus
   IStream::Status status = startTx();
   bool started = false;

   if ( status == IStream::SUCCESS )
   {
      DEBUG_M1( FSTR( "start next trx " ) );
      started = true;
   }

   status = read( (uint8_t*)pData, length, user );
   if ( status != STOPPED && status != ARB_LOST && started )
   {
      stopTx();
   }

   return status;
}


void SoftTwi::handleInterrupt0Source()
{
   uint8_t portStatus = TWI_PORT.IN;

   // try to isolate short peaks that causing an interrupt
   char i = 4;
   while ( --i )
   {
      portStatus |= TWI_PORT.IN;
   }
   if ( !( portStatus & SCL_PIN ) )
   {
      DISABLE_SCL_INT;
      TWI_PORT.OUTCLR = SCL_PIN;
      TWI_PORT.DIRSET = SCL_PIN;
      TRACE_TWI_SCL_HOLD;
   }
}

void SoftTwi::handleInterrupt1Source()
{
   TRACE_TWI_INT1_START;
   uint8_t portStatus = TWI_PORT.IN;

   // try to isolate short peaks that causing an interrupt
   char i = 8;
   while ( --i )
   {
      portStatus &= TWI_PORT.IN;
   }

   if ( portStatus & SCL_PIN )
   {
      if ( portStatus & SDA_PIN )
      {
         TRACE_TWI_STATE_IDLE;
         streamState = IStream::IDLE;
         // disable Int0
         TWI_PORT.INTCTRL &= ~PORT_INT0LVL_gm;
      }
      else
      {
         ENABLE_SCL_INT;
         if ( TWI_PORT.DIR & SDA_PIN )
         {
            TRACE_TWI_STATE_WRITING;
            streamState = IStream::WRITING;
         }
         else
         {
            TRACE_TWI_STATE_READING;
            streamState = IStream::READING;
         }
      }
   }
   TRACE_TWI_INT1_END;
}
