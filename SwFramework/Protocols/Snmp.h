/*
 * Snmp.h
 *
 *  Created on: 03.03.2016
 *      Author: viktor.pankraz
 */

#ifndef PROTOCOLS_SNMP_SNMP_H_
#define PROTOCOLS_SNMP_SNMP_H_

#include <Protocols/Protocols.h>
#include <Protocols/IpStack/IpStackManager.h>

#ifndef SNMP_COMMUNITY_STRING
#define SNMP_COMMUNITY_STRING "public"
#endif

/* Buffer overflow protection :
 *
 * SNMP_MAX_OID_BUFFERSIZE:
 * Maximum stack size allowed to use as OID buffer.
 *
 * SNMP_MAX_BIND_COUNT:
 * Maximum number of bindings per request.
 * This value is used to prevent an overflow of the
 * output buffer. The intend is that the data of
 * the worst case reaction output * SNMP_MAX_BIND_COUNT
 * fit in the network output buffer
 */
#define SNMP_MAX_OID_BUFFERSIZE 128

#define SNMP_MAX_BIND_COUNT     6

/* OID: 1.3.6.1. */
#define SNMP_OID_INTERNET "\x2b\x06\x01"

/* OID: 1.3.6.1.4.1. */
#define SNMP_OID_ENTERPRISES SNMP_OID_INTERNET "\x04\x01"

/* OID: 1.3.6.1.3. */
#define SNMP_OID_EXPERIMENTAL SNMP_OID_INTERNET "\x03"

class Event;

class Snmp
{

   public:
      static const uint16_t SERVER_PORT = 161;
      static const uint16_t MANAGER_PORT = 162;

      enum Versions
      {
         V1 = 0,
         V2,
         V3
      };

      enum Types
      {
         TYPE_BOOLEAN = 0x01,
         TYPE_INTEGER,
         TYPE_BIT_STRING,
         TYPE_OCTED_STRING,
         TYPE_NULL,
         TYPE_OID,
         TYPE_SEQUENCE = 0x30,
         TYPE_IP_ADDRESS = 0x40,
         TYPE_COUNTER,
         TYPE_GAUGE,
         TYPE_TIMETICKS,
         TYPE_OPAQUE,
         TYPE_COUNTER64 = 0x46,
         TYPE_FLOAT = 0x48,
         TYPE_DOUBLE,
         TYPE_INTEGER64,
         TYPE_UINTEGER64,
         TYPE_ENDOFMIB = 0x82,
         TYPE_GETREQ = 0xa0,
         TYPE_GETNEXTREQ,
         TYPE_GETRESP,
         TYPE_SETREQ,
         TYPE_TRAP,
         TYPE_GET_BULK,
         TYPE_INFORM,
         TYPE_TRAP2,
         TYPE_REPORT
      };

      enum Errors
      {
         ERR_NONE,
         ERR_TOO_BIG,
         ERR_NO_SUCH_NAME,
         ERR_BAD_VALUE,
         ERR_READ_ONLY,
         ERR_GEN_ERR,
         ERR_NOACCESS,
         ERR_WRONGTYPE,
         ERR_WRONGLENGTH,
         ERR_WRONGENCODING,
         ERR_WRONGVALUE,
         ERR_NOCREATION,
         ERR_INCONSISTENTVALUE,
         ERR_RESOURCEUNAVAILABLE,
         ERR_COMMITFAILED,
         ERR_UNDOFAILED,
         ERR_AUTHORIZATIONERROR,
         ERR_NOTWRITABLE,
         ERR_INCONSISTENTNAME
      };

      enum GenericTraps
      {
         TRAP_COLDSTART,
         TRAP_WARMSTART,
         TRAP_LINKDOWN,
         TRAP_LINKUP,
         TRAP_AUTHFAIL,
         TRAP_EGPNEIGHBORLOSS,
         TRAP_ENTERPRISESPECIFIC
      };

      class Object
      {

         public:

            static const uint16_t MAX_OBJ_SIZE = 128;

            inline Object() :
               type( TYPE_NULL ), length( 0 )
            {
            }

            inline Object( Types _type ) :
               type( _type ), length( 0 )
            {
            }

            inline void reset()
            {
               type = TYPE_NULL;
               length = 0;
            }

            Object* get( uint8_t idx );

            inline Object* getNext()
            {
               return (Object*) &data[length];
            }

            inline bool isOfType( Types _type )
            {
               return ( type == _type );
            }

            void addObject( Object& obj );

            void setValue( uint32_t value );

            uint32_t getValue32();

            void dump();

            ////    Attributes    ////

         public:
            uint8_t type;
            uint8_t length;
            uint8_t data[MAX_OBJ_SIZE];
      };

      class Integer : public Object
      {

         public:
            inline Integer() :
               Object( TYPE_INTEGER )
            {
            }

            inline Integer( uint32_t value ) :
               Object( TYPE_INTEGER )
            {
               setValue( value );
            }

            inline void reset()
            {
               type = TYPE_INTEGER;
               length = 0;
            }

      };

      class String : public Object
      {

         public:
            inline String() :
               Object( TYPE_OCTED_STRING )
            {
            }

            inline String( const char* string ) :
               Object( TYPE_OCTED_STRING )
            {
               setValue( string, strlen( string ) - 1 );
            }

            inline void setValue( const char* string, uint8_t length )
            {
               memcpy( (char*) data, string, length );
               length = strlen( string );
            }

            inline void reset()
            {
               type = TYPE_OCTED_STRING;
               length = 0;
            }
      };

      class NetworkAddress : public Object
      {

         public:
            inline NetworkAddress() :
               Object( TYPE_IP_ADDRESS )
            {
            }

            inline NetworkAddress( const IP& addr ) :
               Object( TYPE_IP_ADDRESS )
            {
               setIP( addr );
            }

            inline void reset()
            {
               type = TYPE_IP_ADDRESS;
               length = 0;
            }

            inline void setIP( const IP& addr )
            {
               length = 4;
               memcpy( data, &addr, sizeof( addr ) );
            }

            inline IP getIP() const
            {
               if ( length == 4 )
               {
                  IP ip;
                  ip.set( data[0], data[1], data[2], data[3] );
                  return ip;
               }
               return IP::broadcast();
            }

      };

      class Timeticks : public Object
      {

         public:
            inline Timeticks() :
               Object( TYPE_TIMETICKS )
            {
            }

            inline Timeticks( uint32_t value ) :
               Object( TYPE_TIMETICKS )
            {
               setValue( value );
            }

            inline void reset()
            {
               type = TYPE_TIMETICKS;
               length = 0;
            }
      };

      class Sequence : public Object
      {
         ////    Operations    ////
         public:
            inline Sequence() :
               Object( TYPE_SEQUENCE )
            {
            }

            inline void reset()
            {
               type = TYPE_SEQUENCE;
               length = 0;
            }

      };

      class ObjectIdentifier : public Object
      {
         ////    Operations    ////
         public:
            inline ObjectIdentifier() :
               Object( TYPE_OID )
            {
            }

            inline ObjectIdentifier( uint16_t* _oidArray, uint8_t _size ) :
               Object( TYPE_OID )
            {
               setNodes( _oidArray, _size );
            }

            inline void reset()
            {
               type = TYPE_OID;
               length = 0;
            }

            void setNodes( uint16_t* oidArray, uint8_t size );

            uint8_t getNodes( uint16_t* oidArray, uint8_t size );

            ////    Attributes    ////

         protected:

      };

      class VariableBinding : public Sequence
      {
         enum Objects
         {
            OID,
            VALUE
         };

         public:

            inline VariableBinding() :
               Sequence()
            {
            }

            bool hasCorrectOid()
            {
               Object* obj = get( OID );
               return ( obj && obj->isOfType( TYPE_OID ) );
            }

            bool hasCorrectValue( bool isSetRequest )
            {
               Object* obj = get( VALUE );
               return ( obj && ( obj->isOfType( TYPE_NULL ) || isSetRequest ) );
            }
      };

      class PduBody : public Sequence
      {

         public:

            inline PduBody() :
               Sequence()
            {
            }

            inline VariableBinding* getVariableBinding( uint8_t idx )
            {
               return (VariableBinding*) get( idx );
            }

      };

      class PduHeader : public Object
      {
         enum Objects
         {
            REQUEST_ID,
            ERROR_STATUS,
            ERROR_INDEX,
            PDU_BODY
         };

         public:

            inline PduHeader() :
               Object()
            {
            }

            inline bool hasRequest()
            {
               return isOfType( TYPE_SETREQ ) || isOfType( TYPE_GETREQ )
                      || isOfType( TYPE_GETNEXTREQ );
            }

            inline bool hasCorrectRequestId()
            {
               Object* obj = get( REQUEST_ID );
               return ( obj && obj->isOfType( TYPE_INTEGER ) );
            }

            inline bool hasCorrectErrorStatus()
            {
               Object* obj = get( ERROR_STATUS );
               return ( obj && obj->isOfType( TYPE_INTEGER ) );
            }

            inline bool hasCorrectErrorIndex()
            {
               Object* obj = get( ERROR_INDEX );
               return ( obj && obj->isOfType( TYPE_INTEGER ) );
            }

            inline void setErrorStatus( Errors errorStatus )
            {
               get( ERROR_STATUS )->data[0] = errorStatus;
            }

            inline PduBody* getPduBody()
            {
               return (PduBody*) get( PDU_BODY );
            }

      };

      class TrapHeader : public Object
      {
         enum Objects
         {
            DEVICE_OID,
            SENDER_IP,
            GENERAL_TRAP_ID,
            ENTERPRISE_TRAP_ID,
            TIMESTAMP,
            PDU_BODY
         };

         public:

            inline TrapHeader() :
               Object( TYPE_TRAP )
            {
            }

            inline PduBody* getPduBody()
            {
               return (PduBody*) get( PDU_BODY );
            }

      };

      class PaketHeader : public Sequence
      {
         enum Objects
         {
            VERSION,
            COMMUNITY_STRING,
            PDU_HEADER
         };

         ////    Operations    ////
         public:

            inline PaketHeader() :
               Sequence()
            {
            }

            inline bool hasCorrectLength( uint8_t len )
            {
               return ( ( length + 2 ) == len );
            }

            inline bool isVersion( Versions ver )
            {
               Integer* obj = (Integer*) get( VERSION );
               return ( obj && obj->isOfType( TYPE_INTEGER ) && ( obj->length == 1 )
                      && ( obj->getValue32() == ver ) );
            }

            inline bool isCommunityStringCorrect( uint8_t* string )
            {
               return !memcmp( get( COMMUNITY_STRING ), string, string[1] + 2 );
            }

            inline PduHeader* getPduHeader()
            {
               return (PduHeader*) get( PDU_HEADER );
            }

      };

////    Operations    ////

   public:
      Snmp();

      bool notifyEvents( const Event& event );

      bool sendTrapMessage( TrapHeader& header );

      inline void setRemoteIp( const uint32_t& _remoteAddress )
      {
         remoteIp.setAddress( _remoteAddress );
      }

      inline void setCommunityString( const char* _string )
      {
         uint8_t length = strlen( _string );
         if ( length > ( sizeof( communityString ) - 2 ) )
         {
            length = ( sizeof( communityString ) - 2 );
         }
         communityString[0] = TYPE_OCTED_STRING;
         communityString[1] = length;
         memcpy( &communityString[2], _string, length );
      }

   protected:

      uint16_t getNextOidNode( uint8_t*& data );

   private:

      inline bool isRequestSupported( uint8_t request )
      {
         return ( request == TYPE_GETREQ ) || ( request == TYPE_GETNEXTREQ )
                || ( request == TYPE_SETREQ );
      }

      virtual uint8_t bindProc( uint8_t req_type, ObjectIdentifier* oid,
                                Sequence* out ) = 0;

////    Attributes    ////

   protected:

      static const uint8_t debugLevel;

      IP remoteIp;

      uint8_t communityString[18];
};

#endif /* PROTOCOLS_SNMP_SNMP_H_ */
