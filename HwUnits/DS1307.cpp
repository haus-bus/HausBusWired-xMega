/*
 * DS1307.cpp
 *
 *  Created on: 14.03.2016
 *      Author: viktor.pankraz
 */

#include <DS1307.h>
#include <ErrorMessage.h>
#include <SwFramework.h>
#include <Time/Calender.h>

const uint8_t DS1307::debugLevel( DEBUG_LEVEL_OFF );

DS1307::DS1307( Twi& _twi ) :
   twi( &_twi )
{
   setId( ( Object::ClassId::RTC_TIME << 8 ) | 1 );
}

void DS1307::run()
{
   setSleepTime( SystemTime::S );
   uint8_t result = ErrorCode::NO_ERROR;
   TimeElements ts;

   if ( inStartUp() )
   {
      if ( !isRunning() )
      {
         ts.reset();
         result = setTime( ts );

         if ( result != ErrorCode::NO_ERROR )
         {
            ErrorMessage( getId(), result );
         }
      }
      if ( result == ErrorCode::NO_ERROR )
      {
         // enable square output with 8.192 kHz
         result = setControlRegister( 0x12 );

         if ( result == ErrorCode::NO_ERROR )
         {
            result = getTime( ts );
            if ( result == ErrorCode::NO_ERROR )
            {
               Calender::now = Calender( ts.seconds, ts.minutes, ts.hours,
                                         ts.date - 1, ts.month - 1, ts.year,
                                         ts.dayOfWeek - 1 );
               SystemTime::init( SystemTime::RTCSRC_EXTCLK, 8192 );
               SET_STATE_L1( RUNNING );
            }
         }
      }
   }
   else if ( inRunning() )
   {
      result = getTime( ts );
      if ( result != ErrorCode::NO_ERROR )
      {
         SET_STATE_L1( FAILTURE );
         ErrorMessage( getId(), result );
      }
      else
      {
         // monitor for time deviation
         Calender Ds1307Calender( ts.seconds, ts.minutes, ts.hours, ts.date - 1,
                                  ts.month - 1, ts.year, ts.dayOfWeek - 1 );

         DEBUG_H2( FSTR( "Calender deviation: 0x" ),
                   Calender::now.getDifferenceTo( Ds1307Calender ).toTimestamp() );

      }
      setSleepTime( SystemTime::MIN );
   }
   else
   {
      result = getTime( ts );
      if ( result == ErrorCode::NO_ERROR )
      {
         SET_STATE_L1( RUNNING );
         ErrorMessage( getId(), result );
      }
   }
}

bool DS1307::notifyEvent( const Event& event )
{
   bool consumed = true;

   if ( event.isEvWakeup() )
   {
      run();
   }
   else if ( event.isEvMessage() )
   {
      HACF* message = event.isEvMessage()->getMessage();
      HACF::ControlFrame& cf = message->controlFrame;
      Command* data = reinterpret_cast<Command*>( cf.getData() );

      Response response( getId(), *message );
      uint8_t errorCode = ErrorCode::NO_ERROR;

      if ( cf.isCommand( Command::SET_TIME ) )
      {
         errorCode = setTime( data->getParameter().te );
         consumed = true;
      }
      else if ( cf.isCommand( Command::GET_TIME ) )
      {
         errorCode = getTime( response.setTime().te );
         if ( errorCode == ErrorCode::NO_ERROR )
         {
            response.queue( getObject( message->header.getSenderId() ) );
         }
         consumed = true;
      }
      else
      {
         consumed = false;
      }

      if ( errorCode != ErrorCode::NO_ERROR )
      {
         response.setErrorCode( errorCode );
         response.queue( getObject( message->header.getSenderId() ) );
      }
   }
   else
   {
      consumed = false;
   }

   return consumed;
}

uint8_t DS1307::setTime( DS1307::TimeElements& timeElements )
{
   DEBUG_H1( FSTR( ".setTime()" ) );
   if ( twi->isBusBusy() )
   {
      WARN_1( FSTR( "twi is busy" ) );
      return ErrorCode::TWI_BUSY;
   }

   uint8_t buffer[] = { 0,                // reset register pointer
                        dec2bcd( timeElements.seconds ),
                        dec2bcd( timeElements.minutes ),
                        dec2bcd( timeElements.hours ), // sets 24 hour format
                        dec2bcd( timeElements.dayOfWeek ),
                        dec2bcd( timeElements.date ),
                        dec2bcd( timeElements.month ),
                        dec2bcd( timeElements.year % 100 ) };

   if ( twi->master.write( ADDRESS, buffer, sizeof( buffer ) ) != sizeof( buffer ) )
   {
      ERROR_1( FSTR( "twi write failed" ) );
      return ErrorCode::TWI_WRITE_FAILED;
   }
   return ErrorCode::NO_ERROR;
}

uint8_t DS1307::getTime( DS1307::TimeElements& timeElements )
{
   DEBUG_H1( FSTR( ".getTime()" ) );
   if ( twi->isBusBusy() )
   {
      WARN_1( FSTR( "twi is busy" ) );
      return ErrorCode::TWI_BUSY;
   }

   uint8_t buffer[sizeof( timeElements )];

   // reset register pointer
   buffer[0] = 0;
   if ( twi->master.write( ADDRESS, buffer, 1, false ) != 1 )
   {
      ERROR_1( FSTR( "reset register failed" ) );
      return ErrorCode::TWI_WRITE_FAILED;
   }

   // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
   if ( twi->master.read( ADDRESS, buffer, 7 ) != 7 )
   {
      ERROR_1( FSTR( "twi read failed" ) );
      return ErrorCode::TWI_READ_FAILED;
   }

   timeElements.seconds = bcd2dec( buffer[0] & 0x7f );
   timeElements.minutes = bcd2dec( buffer[1] );
   timeElements.hours = bcd2dec( buffer[2] & 0x3f ); // mask assumes 24hr clock
   timeElements.dayOfWeek = bcd2dec( buffer[3] );
   timeElements.date = bcd2dec( buffer[4] );
   timeElements.month = bcd2dec( buffer[5] );
   timeElements.year = 2000 + bcd2dec( buffer[6] );

   if ( buffer[0] & 0x80 )
   {
      ERROR_1( FSTR( "clock is halted" ) );
      return ErrorCode::CLOCK_HALTED;
   }
   return ErrorCode::NO_ERROR;
}

bool DS1307::isRunning()
{
   DEBUG_H1( FSTR( ".isRunning()" ) );
   if ( twi->isBusBusy() )
   {
      return false;
   }

   uint8_t buffer;

   // reset register pointer
   buffer = 0;

   if ( twi->master.write( ADDRESS, &buffer, sizeof( buffer ), false )
        != sizeof( buffer ) )
   {
      ERROR_1( FSTR( "reset register failed" ) );
      return false;
   }

   if ( twi->master.read( ADDRESS, &buffer, sizeof( buffer ) ) != sizeof( buffer ) )
   {
      ERROR_1( FSTR( "twi read failed" ) );
      return ErrorCode::TWI_READ_FAILED;
   }

   return !( buffer & 0x80 );
}

uint8_t DS1307::setControlRegister( uint8_t data )
{
   DEBUG_H1( FSTR( ".setControlRegister()" ) );
   if ( twi->isBusBusy() )
   {
      WARN_1( FSTR( "twi is busy" ) );
      return ErrorCode::TWI_BUSY;
   }

   uint8_t buffer[] = { 0x07, data };

   if ( twi->master.write( ADDRESS, buffer, sizeof( buffer ) ) != sizeof( buffer ) )
   {
      ERROR_1( FSTR( "twi write failed" ) );
      return ErrorCode::TWI_WRITE_FAILED;
   }
   return ErrorCode::NO_ERROR;
}

DS1307::Response::Parameter& DS1307::Response::setTime()
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().te ) );
   setResponse( TIME );
   return getParameter();
}
