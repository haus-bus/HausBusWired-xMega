/*
 * DaliLine.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "DaliLine.h"

const uint8_t DaliLine::debugLevel( DEBUG_LEVEL_OFF );

DaliLine::Response::Parameter& DaliLine::Response::setConfiguration()
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().configuration) );
  setResponse( CONFIGURATION );
  return getParameter();
}

void DaliLine::Response::setStatus( uint8_t status )
{
  controlFrame.setDataLength(
      sizeof(getResponse()) + sizeof(getParameter().status) );
  setResponse( Response::STATUS );
  getParameter().status = status;
}

DaliLine::DaliLine( Dali& _daliHw, uint8_t _id ) :
    daliHw( _daliHw )
{
  configuration = NULL;
  setId( (ClassId::DALI_LINE << 8) | _id );
  setConfiguration( HwConfiguration::getDaliLineConfiguration( id ) );
}

bool DaliLine::notifyEvent( const Event& event )
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

void DaliLine::run()
{
}

bool DaliLine::handleRequest( HACF* message )
{
  if ( !message->controlFrame.isCommand() )
  {
    return false;
  }
  HACF::ControlFrame& cf = message->controlFrame;
  Command* data = reinterpret_cast<Command*>( cf.getData() );

  if ( cf.isCommand( Command::ALL_ON ) )
  {
    DEBUG_H1( FSTR(".allOn()") );
    uint8_t cmd[2] = { 0xFE, 0xFE };
    daliHw.write( cmd );
  }
  else if ( cf.isCommand( Command::ALL_OFF ) )
  {
    DEBUG_H1( FSTR("(.allOff()") );
    uint8_t cmd[2] = { 0xFE, 0 };
    daliHw.write( cmd );
  }
  else if ( cf.isCommand( Command::SET_CONFIGURATION ) )
  {
    DEBUG_H1( FSTR("(.allOff()") );
    configuration->set( data->parameter.setConfiguration );
  }
  else
  {
    Response response( getId(), *message );

    if ( cf.isCommand( Command::SEND_CMD ) )
    {
      DEBUG_H1( FSTR(".sendCmd()") );
      daliHw.write( cf.getData() );
      if ( (data->parameter.sendCmd.command >= Dali::RESET)
          && (data->parameter.sendCmd.command <= Dali::STORE_DTR_AS_SCHORT_ADDR) )
      {
        // repeat command
        SystemTime::waitMs( 20 );
        daliHw.write( cf.getData() );
      }
      uint8_t reply;
      uint8_t status = daliHw.read( &reply );

      if ( status == Stream::SUCCESS )
      {
        response.setStatus( reply );
      }
      else
      {
        response.setErrorCode( status );
      }
    }
    else if ( cf.isCommand( Command::GET_CONFIGURATION ) )
    {
      DEBUG_H1( FSTR(".getConfiguration()") );
      configuration->get( response.setConfiguration().configuration );
      response.queue( getObject( message->header.getSenderId() ) );
      return true;
    }
    else
    {
      return false;
    }
    response.queue();
  }
  return true;
}

HwConfiguration::DaliLine* DaliLine::getConfiguration() const
{
  return configuration;
}

void DaliLine::setConfiguration( HwConfiguration::DaliLine* p_DaliLine )
{
  configuration = p_DaliLine;
}
