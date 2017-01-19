/*
 * HacfIpStackManager.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwUnits_HacfIpStackManager_H
#define SwUnits_HacfIpStackManager_H

#include "SwUnits.h"
#include "HwConfiguration.h"
#include <Protocols/IpStack/IpStackManager.h>
class ArpHeader;

class ArpManager;

class CriticalSection;

class Dhcp;

class Enc28j60;

class Event;

class HACF;

class IP;

class IcmpHeader;

class MAC;

class ModBusSlave;

class Scheduler;

class TcpConnection;

class UdpConnection;

class UdpHeader;

class evMessage;

class HacfIpStackManager: public IpStackManager
{
public:

  class Command;

  class Response;

  class Command
  {
  public:

    enum Commands
    {
      GET_CONFIGURATION = HACF::COMMANDS_START,
      SET_CONFIGURATION,
      WAKE_UP_DEVICE,
      GET_CURRENT_IP
    };

    union Parameter
    {
      uint8_t mac[6];
      HwConfiguration::Ethernet setConfiguration;
    };

    ////    Operations    ////

    uint8_t getCommand() const;

    inline Parameter& getParameter()
    {
      return parameter;
    }

    void setCommand( uint8_t p_command );

    void setParameter( const Parameter& p_parameter );

    ////    Attributes    ////

    uint8_t command;

    Parameter parameter;
  };

  class Response: public IResponse
  {
  public:

    enum Responses
    {
      CONFIGURATION = HACF::RESULTS_START,
      CURRENT_IP
    };

    union Parameter
    {
      HwConfiguration::Ethernet configuration;
      uint32_t ip;
    };

    ////    Constructors and destructors    ////

    inline Response( uint16_t id ) :
        IResponse( id )
    {
    }

    inline Response( uint16_t id, const HACF& message ) :
        IResponse( id, message )
    {
    }

    ////    Operations    ////

    inline Parameter& getParameter()
    {
      return *reinterpret_cast<Parameter*>( IResponse::getParameter() );
    }

    Parameter& setConfiguration();

    void setCurrentIp();

    ////    Attributes    ////

  private:

    Parameter params;
  };

  ////    Constructors and destructors    ////

  HacfIpStackManager( Enc28j60& _stream );

  ////    Operations    ////

  HwConfiguration::Ethernet* getConfiguration() const;

  virtual bool notifyEvent( const Event& event );

  void setConfiguration( HwConfiguration::Ethernet* p_Ethernet );

  void wakeUpDevice( const MAC& mac );

protected:

  bool handleRequest( HACF* message );

  ////    Relations and components    ////

  HwConfiguration::Ethernet* configuration;
};

#endif
