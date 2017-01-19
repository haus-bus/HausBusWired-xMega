/*
 * LoggerMemory.cpp
 *
 *  Created on: 11.04.2016
 *      Author: viktor.pankraz
 */

#include <LoggerMemory.h>
#include <Gateway.h>
#include <SwFramework.h>
#include <Time/Calender.h>
#include <ErrorMessage.h>

const uint8_t LoggerMemory::debugLevel( DEBUG_LEVEL_LOW | DEBUG_STATE_L3 );

char LoggerMemory::logFileName[] = "0:2016/0101.has";

LoggerMemory::LoggerMemory( IDiskIo* disk )
{
  setId( (Object::ClassId::LOGGER_MEMORY << 8) | 1 );
  myFileSystem = new FatSystem::FileSystem( disk );
  if ( !myFileSystem )
  {
    terminate();
    ErrorMessage( getId(), ErrorMessage::HEAP_OUT_OF_MEMORY );
  }
}

void LoggerMemory::run()
{
  setSleepTime( SystemTime::S );
  FatSystem::Result res;

  if ( inStartUp() )
  {
    DEBUG_M1( FSTR("mount drive 0") )
    res = myFileSystem->mount( 0 );
    if ( res != FatSystem::OK )
    {
      DEBUG_L1( FSTR( " failed" ) );
      ErrorMessage( getId(), res );
    }
    else
    {
      SET_STATE_L1( RUNNING );
    }

  }
  else if ( inRunning() )
  {
    if ( updateLogfileName() || !myLogFile.isOpen() )
    {
      res = FatSystem::OK;

      if ( myLogFile.isOpen() )
      {
        res = myLogFile.close();
        if ( res != FatSystem::OK )
        {
          ERROR_2( FSTR("close failed with error 0x"), (uint8_t )res );
          ErrorMessage( getId(), res );
          SET_STATE_L1( FAILTURE );
          return;
        }
      }

      res = prepareLogFile();
      if ( res != FatSystem::OK )
      {
        ERROR_2( FSTR("open failed with error 0x"), (uint8_t )res );
        ErrorMessage( getId(), res );
        SET_STATE_L1( FAILTURE );
        return;
      }
    }
    res = myLogFile.sync();
    if ( res != FatSystem::OK )
    {
      ERROR_2( FSTR("sync failed with error 0x"), (uint8_t )res );
      ErrorMessage( getId(), res );
      SET_STATE_L1( FAILTURE );
      return;
    }

    Gateway::setListener( this );

  }
  else if ( inFailture() )
  {

  }
}

void LoggerMemory::sendError( ErrorCode::ErrorCodes code )
{
  IResponse event( getId() );
  event.setErrorCode( code );
  event.queue();
}

bool LoggerMemory::notifyEvent( const Event& event )
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

    if ( cf.isCommand( Command::ERASE ) )
    {
      DEBUG_H1( FSTR("ERASE") );
    }
    else if ( cf.isCommand( Command::GET_LOGS ) )
    {
      DEBUG_H2( FSTR("GET_LOGS at sector 0x"), data->parameter.sector );
    }
    else
    {
      consumed = false;
    }
  }
  else if ( event.isEvGatewayMessage() )
  {
    DEBUG_M1( FSTR( "writing to logFile" ) );
    HACF* message = event.isEvGatewayMessage()->getMessage();

    uint16_t written;
    FatSystem::Result res = myLogFile.write( message, message->getLength(),
                                             written );
    if ( res != FatSystem::OK )
    {
      ERROR_2( FSTR("write failed with error 0x"), (uint8_t )res );
      ErrorMessage( getId(), res );
      Gateway::setListener( NULL );
      SET_STATE_L1( FAILTURE );
    }
  }
  else
  {
    consumed = false;
  }

  return consumed;
}

LoggerMemory::Response::Parameter& LoggerMemory::Response::setLogData()
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().logData) );
  setResponse( LOG_DATA );
  return getParameter();
}


bool LoggerMemory::updateLogfileName()
{
  // filename format is always 0:yyyy/mmdd.has
  char fileName[] = "0:0000/0000.has";
  uint16_t year = Calender::now.getYear();

  for ( uint8_t i = 5; i > 1; i-- )
  {
    fileName[i] = (year % 10) + '0';
    year /= 10;
  }

  uint8_t date = Calender::now.getMonth()+1;
  fileName[8] = (date % 10) + '0';
  date /= 10;
  fileName[7] = date + '0';

  date = Calender::now.getDate()+1;
  fileName[10] = (date % 10) + '0';
  date /= 10;
  fileName[9] = date + '0';

  int result = strcmp( fileName, logFileName );
  strcpy( logFileName, fileName );
  return result;
}

FatSystem::Result LoggerMemory::prepareLogFile()
{
  // cut first to directory path only
  logFileName[6] = 0;
  FatSystem::Result res = FatSystem::Directory::create( logFileName );
  if ( res != FatSystem::OK && res != FatSystem::EXIST )
  {
    ERROR_4( FSTR("create directory: "), logFileName,
             FSTR(" failed with error 0x"), (uint8_t )res );
    return res;
  }

  // restore full path
  logFileName[6] = '/';
  res = myLogFile.open(
      logFileName,
      FatSystem::File::FA_OPEN_ALWAYS | FatSystem::File::FA_WRITE );
  if ( res != FatSystem::OK )
  {
    ERROR_4( FSTR( "open " ), logFileName, FSTR(" failed with error 0x"),
             (uint8_t )res );
    return res;
  }

  res = myLogFile.seek( myLogFile.getSize() );
  if ( res != FatSystem::OK )
  {
    ERROR_4( FSTR( "seek to pos 0x" ), myLogFile.getSize(),
             FSTR(" failed with error 0x"), (uint8_t )res );
    myLogFile.close();
    return res;
  }

  return res;
}
