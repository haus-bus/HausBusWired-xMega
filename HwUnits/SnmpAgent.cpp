/*
 * SnmpAgent.cpp
 *
 *  Created on: 03.03.2016
 *      Author: viktor.pankraz
 */

#include <SnmpAgent.h>
#include <PortPinUnit.h>
#include <Button.h>
#include <Protocols/IpStack/UdpConnection.h>
#include <HomeAutomationInterface.h>

#define MAX_SEND_RETRIES 10

// OID: 1.3.6.1.4.1.47408.1
#define SNMP_OID_PANKRAZ_ELEKTRONIK SNMP_OID_ENTERPRISES "\x82\xf2\x30\x01"
extern const uint8_t oidData[] PROGMEM;

const uint8_t oidData[] = { SNMP_OID_PANKRAZ_ELEKTRONIK };

const uint8_t SnmpAgent::debugLevel( DEBUG_LEVEL_OFF );

SnmpAgent::Response::Parameter& SnmpAgent::Response::setConfiguration()
{
   controlFrame.setDataLength(
      sizeof( getResponse() ) + sizeof( getParameter().configuration ) );
   setResponse( CONFIGURATION );
   return getParameter();
}

SnmpAgent::SnmpAgent()
{
   setId( ( ::Object::ClassId::SMTP_AGENT << 8 ) | 1 );
   configuration = HwConfiguration::getSnmpAgentConfiguration( id );
}

bool SnmpAgent::handleRequest( HACF* message )
{
   if ( !message->controlFrame.isCommand() )
   {
      return false;
   }
   HACF::ControlFrame& cf = message->controlFrame;
   Command* data = reinterpret_cast<Command*>( cf.getData() );

   if ( cf.isCommand( Command::SET_CONFIGURATION ) )
   {
      DEBUG_H1( FSTR( ".setConfiguration()" ) );
      configuration->set( data->parameter.setConfiguration );
      updateConfiguration();

   }
   else if ( cf.isCommand( Command::GET_CONFIGURATION ) )
   {
      DEBUG_H1( FSTR( ".getConfiguration()" ) );
      Response response( getId(), *message );
      configuration->get( response.setConfiguration().configuration );
      response.queue( getObject( message->header.getSenderId() ) );
   }
   else
   {
      return false;
   }
   return true;
}

bool SnmpAgent::notifyEvent( const Event& event )
{
   if ( event.isEvWakeup() )
   {
      run();
   }
   else if ( event.isEvMessage() )
   {
      DEBUG_H1( FSTR( ".evMessage" ) );
      return handleRequest( event.isEvMessage()->getMessage() );
   }
   else if ( event.isEvGatewayMessage() )
   {
      DEBUG_H1( FSTR( ".evGatewayMessage" ) );
      HACF* message = event.isEvGatewayMessage()->getMessage();
      HACF::ControlFrame& cf = message->controlFrame;

      if ( cf.isEvent() )
      {
         HACF::Object& sender = cf.senderId;
         if ( isClassIdSupported( sender.classId ) )
         {
            sendMessage( message );
// if ( !itsMessageQueue.isEmpty() || !sendMessage( message ) )
// {
// if ( itsMessageQueue.push( message ) )
// {
// setSleepTime( WAKE_UP );
// return true;
// }
// ERROR_1( FSTR("SnmpAgent::messageQueue overrun") );
// }
         }
      }
      delete message;
   }
   else if ( event.isEvData() )
   {
      return Snmp::notifyEvents( event );
   }
   return false;
}

uint8_t SnmpAgent::bindProc( uint8_t req_type, Snmp::ObjectIdentifier* oid,
                             Snmp::Sequence* out )
{
   DEBUG_H1( FSTR( ".bindProc()" ) );
   Reactive* receiver = NULL;

   uint8_t i = sizeof( oidData ) - 1;
   if ( memcmp_P( oid->data, oidData, i ) != 0 )
   {
      if ( req_type != Snmp::TYPE_GETNEXTREQ )
      {
         ERROR_1( FSTR( "oid not found" ) );
         return 0;
      }
      HACF::Object object;
      receiver = getNextNode( object );
      if ( !receiver )
      {
         ERROR_1( FSTR( "oid not found for TYPE_GETNEXTREQ" ) );
         return 0;
      }
   }

   uint16_t nodes[MAX_NODES];
   if ( receiver )
   {
      nodes[ISO] = 1;
      nodes[ORG] = 3;
      nodes[DOD] = 6;
      nodes[INTERNET] = 1;
      nodes[PRIVATE] = 4;
      nodes[ENTERPRISES] = 1;
      nodes[PANKRAZ] = 47408;
      nodes[PRODUCTS] = 1;
      nodes[DEVICE_TYPE] = 4;
      nodes[DEVICE_ID] = HACF::deviceId;
      nodes[CLASS_ID] = receiver->getClassId();
      nodes[INSTANCE_ID] = receiver->getInstanceId();
      nodes[FUNCTION_ID] = 1;
   }
   else
   {
      uint8_t numOfNodes = oid->getNodes( nodes, MAX_NODES );

      if ( ( ( numOfNodes <= DEVICE_TYPE ) || ( nodes[DEVICE_TYPE] != 4 ) )
         && ( req_type == Snmp::TYPE_GETNEXTREQ ) )
      {
         nodes[DEVICE_TYPE] = 4;
      }

      if ( ( ( numOfNodes <= DEVICE_ID ) || ( nodes[DEVICE_ID] != HACF::deviceId ) )
         && ( req_type == Snmp::TYPE_GETNEXTREQ ) )
      {
         nodes[DEVICE_ID] = HACF::deviceId;
      }

      if ( ( ( numOfNodes <= CLASS_ID ) || !isClassIdSupported( nodes[CLASS_ID] ) )
         && ( req_type == Snmp::TYPE_GETNEXTREQ ) )
      {
         nodes[CLASS_ID] = getFirstSupportedClassId();
      }

      HACF::Object object;
      object.setDeviceId( nodes[DEVICE_ID] );
      object.setClassId( nodes[CLASS_ID] );
      object.setInstanceId( nodes[INSTANCE_ID] );

      if ( nodes[DEVICE_TYPE] == 4 ) // IO128
      {
         if ( nodes[DEVICE_ID] == HACF::deviceId )
         {
            receiver = getObject( object.getObjectId() );
            if ( req_type == Snmp::TYPE_GETNEXTREQ )
            {
               receiver = getNextNode( object );
               nodes[FUNCTION_ID] = 1;
            }
            if ( !receiver )
            {
               ERROR_1( FSTR( "oid wrong ObjectId" ) );
            }
         }
         else
         {
            ERROR_1( FSTR( "oid wrong deviceID" ) );
         }
      }
      else
      {
         ERROR_1( FSTR( "oid wrong product" ) );
      }

      if ( !receiver )
      {
         ERROR_1( FSTR( "oid not found" ) );
         return 0;
      }
      nodes[CLASS_ID] = receiver->getClassId();
      nodes[INSTANCE_ID] = receiver->getInstanceId();
   }

   DEBUG_M1( FSTR( "receiver found!" ) );
   out->reset();
   Snmp::ObjectIdentifier myOid( nodes, MAX_NODES );
   out->addObject( myOid );

   switch ( receiver->getClassId() )
   {
      case ::Object::ClassId::DIGITAL_OUTPUT:
         if ( req_type == TYPE_SETREQ )
         {
            if ( nodes[FUNCTION_ID] == 1 )
            {
               Snmp::Object* obj = oid->getNext();
               if ( obj->isOfType( TYPE_INTEGER ) )
               {
                  IResponse cmd( getId() );
                  cmd.controlFrame.receiverId.setDeviceId( HACF::deviceId );
                  cmd.controlFrame.receiverId.setObjectId( receiver->getId() );
                  if ( obj->data[0] != 0 )
                  {
                     cmd.controlFrame.data[0] = PortPinUnit::Command::ON;
                     cmd.controlFrame.data[1] = 0;
                     cmd.controlFrame.data[2] = 0;
                  }
                  else
                  {
                     cmd.controlFrame.data[0] = PortPinUnit::Command::OFF;
                  }
                  out->addObject( *obj );
                  notifyMessageReceived( &cmd.controlFrame );
                  // receiver->notifyEvent( evMessage( receiver, &cmd ) );
               }
               else
               {
                  ERROR_1( FSTR( "wrong data type" ) );
                  return 0;
               }
            }
            else
            {
               ERROR_1( FSTR( "oid(functionId) not found" ) );
               return 0;
            }
         }
         else if ( ( req_type == TYPE_GETREQ ) || ( req_type == TYPE_GETNEXTREQ ) )
         {
            if ( nodes[FUNCTION_ID] == 1 )
            {
               Snmp::Integer outpuState;
               if ( receiver->inSubState( PortPinUnit::OFF ) )
               {
                  DEBUG_M1( FSTR( "PortPin is off" ) );
                  outpuState.setValue( 0 );
               }
               else
               {
                  DEBUG_M1( FSTR( "PortPin is on" ) );
                  outpuState.setValue( 1 );
               }
               out->addObject( outpuState );
            }
            else
            {
               ERROR_1( FSTR( "oid(functionId) not found" ) );
               return 0;
            }
         }
         else
         {
            ERROR_2( FSTR( "request not supported_ 0x" ), req_type );
            return 0;
         }
         break;

      case ::Object::ClassId::BUTTON:
         if ( ( req_type == TYPE_GETREQ ) || ( req_type == TYPE_GETNEXTREQ ) )
         {
            if ( nodes[FUNCTION_ID] == 1 )
            {
               Snmp::Integer buttonState;
               if ( receiver->inSubState( Button::RELEASED )
                  || receiver->inSubState( Button::CLICKED ) )
               {
                  DEBUG_M1( FSTR( "Button is released" ) );
                  buttonState.setValue( 0 );
               }
               else
               {
                  DEBUG_M1( FSTR( "Button is pressed" ) );
                  buttonState.setValue( 1 );
               }
               out->addObject( buttonState );
            }
            else
            {
               ERROR_1( FSTR( "oid(functionId) not found" ) );
               return 0;
            }
         }
         else
         {
            ERROR_1( FSTR( "request not supported" ) );
            return 0;
         }
         break;

      default:
         ERROR_3( FSTR( "classId 0x" ), receiver->getClassId(),
                  FSTR( " not supported" ) )
         ;
   }

   return out->length + 2;
}

void SnmpAgent::run()
{
   if ( inStartUp() )
   {
      if ( !UdpConnection::listen( SERVER_PORT, this ) )
      {
         terminate();
         ERROR_3( FSTR( "listening on udp port " ), SERVER_PORT,
                  FSTR( " not possible!" ) );
      }
      updateConfiguration();

      SET_STATE_L1( RUNNING );

   }

   if ( inRunning() )
   {
// static uint8_t retries = MAX_SEND_RETRIES;

// if ( !itsMessageQueue.isEmpty() )
// {
// HACF* message = itsMessageQueue.front();
// if ( sendMessage( message ) || !--retries )
// {
// retries = MAX_SEND_RETRIES;
// itsMessageQueue.pop( message );
// delete message;
// }
// setSleepTime( 100 * SystemTime::MS );
// }
// else
      {
         // setSleepTime( NO_WAKE_UP );
         setSleepTime( 100 * SystemTime::MS );
      }
   }
}

bool SnmpAgent::sendMessage( HACF* msg )
{
   HACF::ControlFrame& cf = msg->controlFrame;
   HACF::Object& sender = cf.senderId;

   uint16_t senderOidNodes[] = { 1, 3, 6, 1, 4, 1, 47408, 1, 4,
                                 sender.getDeviceId(), 0, sender.instanceId, 1 };
   Snmp::ObjectIdentifier senderOid( senderOidNodes, 11 );

   bool isEvStarted = ( sender.classId == ::Object::ClassId::SYSTEM )
                      && ( sender.instanceId == 1 )
                      && ( cf.data[0] == HomeAutomationInterface::Response::EVENT_STARTED );

   Snmp::TrapHeader header;
   header.addObject( senderOid );
   Snmp::NetworkAddress netIp( IP::local );
   header.addObject( netIp );
   Snmp::Integer snmpInteger(
      isEvStarted ? TRAP_COLDSTART : TRAP_ENTERPRISESPECIFIC );
   header.addObject( snmpInteger ); // GENERAL_TRAP_ID
   snmpInteger.setValue( 0 );
   header.addObject( snmpInteger ); // ENTERPRISE_TRAP_ID
   Snmp::Timeticks timeticks( SystemTime::now() / 10 );
   header.addObject( timeticks );

   if ( ( sender.classId == ::Object::ClassId::BUTTON )
      || ( sender.classId == ::Object::ClassId::DIGITAL_OUTPUT ) )
   {
      Snmp::Sequence seq;
      senderOidNodes[CLASS_ID] = sender.classId;
      senderOid.setNodes( senderOidNodes, sizeof( senderOidNodes ) / 2 );
      seq.addObject( senderOid );
      if ( sender.classId == ::Object::ClassId::BUTTON )
      {
         snmpInteger.setValue(
            ( cf.data[0] == Button::Response::EVENT_FREE ) ? 0 : 1 );
      }
      else if ( sender.classId == ::Object::ClassId::DIGITAL_OUTPUT )
      {
         snmpInteger.setValue(
            ( cf.data[0] == PortPinUnit::Response::EVENT_OFF ) ? 0 : 1 );
      }
      seq.addObject( snmpInteger );

      Snmp::VariableBinding varBind;
      varBind.addObject( seq );
      header.addObject( varBind );
   }

   return sendTrapMessage( header );
}

void SnmpAgent::updateConfiguration()
{
   if ( configuration )
   {
      char communityString[32];
      configuration->getCommunityString( communityString );
      setRemoteIp( IP( configuration->getTrapDestinationIp() ) );
      setCommunityString( communityString );
   }
}

Reactive* SnmpAgent::getNextNode( HACF::Object& object )
{
   DEBUG_H1( FSTR( ".getNextReceiver()" ) );
   DEBUG_M2( FSTR( "start with object: 0x" ), object.getObjectId() );

   uint8_t i = 0;
   uint8_t nextInstanceId = 0xFF;
   Reactive* nextReceiver;

   if ( !isClassIdSupported( object.getClassId() )
      || ( object.getClassId() < getFirstSupportedClassId() ) )
   {
      DEBUG_M2( FSTR( "set first supported class: 0x" ),
                getFirstSupportedClassId() );
      object.setClassId( getFirstSupportedClassId() );
      object.setInstanceId( 0 );
   }

   do
   {
      nextReceiver = getNextOfClass( object.getClassId(), i );
      if ( nextReceiver )
      {
         if ( ( nextReceiver->getInstanceId() > object.getInstanceId() )
            && ( nextReceiver->getInstanceId() < nextInstanceId ) )
         {
            nextInstanceId = nextReceiver->getInstanceId();
            DEBUG_M2( FSTR( "nextInstanceId: 0x" ), nextInstanceId );
         }
      }
      else if ( nextInstanceId == 0xFF )
      {
         if ( object.getClassId() == ::Object::ClassId::BUTTON )
         {
            DEBUG_M1( FSTR( "switch to class DIGITAL_OUTPUT" ) );
            object.setClassId( ::Object::ClassId::DIGITAL_OUTPUT );
            object.setInstanceId( 0 );
            i = 0;
         }
      }

   }
   while ( nextReceiver || ( i == 0 ) );

   object.setInstanceId( nextInstanceId );
   DEBUG_M2( FSTR( "return with object: 0x" ), object.getObjectId() );
   return getObject( object.getObjectId() );
}
