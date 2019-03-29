/*
 * Snmp.cpp
 *
 *  Created on: 03.03.2016
 *      Author: viktor.pankraz
 */

#include "Snmp.h"

#include <EventPkg/EventPkg.h>
#include <Protocols/IpStack/IpConnection.h>
#include <SwFramework.h>
#include <IStream.h>

const uint8_t Snmp::debugLevel( DEBUG_LEVEL_OFF );

#define getId() FSTR( "Snmp::" )

Snmp::Snmp()
{
   communityString[0] = TYPE_OCTED_STRING;
   communityString[1] = 6;
   communityString[2] = 'p';
   communityString[3] = 'u';
   communityString[4] = 'b';
   communityString[5] = 'l';
   communityString[6] = 'i';
   communityString[7] = 'c';
}

bool Snmp::notifyEvents( const Event& event )
{
   DEBUG_H1( FSTR( "notifyEvent()" ) );

   if ( event.isEvData() )
   {
      IStream::TransferDescriptor* td = event.isEvData()->getTransferDescriptor();

      if ( td->bytesTransferred != 0 )
      {
         // we received new data
         DEBUG_M1( FSTR( "command received" ) );

         // reset transfer descriptor
         uint16_t bytesTransferred = td->bytesTransferred;
         td->bytesTransferred = 0;

         PaketHeader* hdr = (PaketHeader*) td->pData;
         if ( !hdr->isOfType( TYPE_SEQUENCE ) )
         {
            ERROR_1( FSTR( "is not TYPE_SEQUENCE" ) );
            return false;
         }

         if ( !hdr->hasCorrectLength( bytesTransferred ) )
         {
            ERROR_1( FSTR( " sequence has wrong length" ) );
            return false;
         }

         if ( !hdr->isVersion( V1 ) )
         {
            ERROR_1( FSTR( "version not supported" ) );
            return false;
         }

         if ( !hdr->isCommunityStringCorrect( communityString ) )
         {
            ERROR_1( FSTR( "wrong community string" ) );
            return false;
         }

         PduHeader* pduHdr = hdr->getPduHeader();
         if ( !pduHdr )
         {
            ERROR_1( FSTR( "invalid PduHeader" ) );
            return false;
         }
         if ( !pduHdr->hasRequest() )
         {
            ERROR_1( FSTR( "invalid request" ) );
            return false;
         }

         if ( !pduHdr->hasCorrectRequestId() )
         {
            ERROR_1( FSTR( "invalid requestID" ) );
            return false;
         }

         if ( !pduHdr->hasCorrectErrorStatus() )
         {
            ERROR_1( FSTR( " invalid error-status" ) );
            return false;
         }

         if ( !pduHdr->hasCorrectErrorIndex() )
         {
            ERROR_1( FSTR( "invalid error-index" ) );
            return false;
         }

         PduBody* pduBody = pduHdr->getPduBody();
         if ( !pduBody )
         {
            ERROR_1( FSTR( "invalid PduBody" ) );
            return false;
         }
         if ( !pduBody->isOfType( TYPE_SEQUENCE ) )
         {
            ERROR_1( FSTR( "no sequence in PDU-BODY" ) );
            return false;
         }

         if ( pduBody->length > SNMP_MAX_OID_BUFFERSIZE )
         {
            ERROR_1( FSTR( "PDU-BODY too long" ) );
            return false;
         }

         // copy varbind list since we build the answer in place
         uint8_t vbList[pduBody->length + 2];
         memcpy( vbList, pduBody, sizeof( vbList ) );

         // process bindings
         DEBUG_M2( FSTR( "pduBody:" ), endl );
         pduBody->dump();

         uint8_t i = 0;
         uint8_t* out = pduBody->data;
         int8_t len_delta = 0;
         pduBody = (PduBody*) vbList;
         VariableBinding* varBind = pduBody->getVariableBinding( i );

         while ( varBind )
         {
            DEBUG_M4( FSTR( "varBind(" ), i, FSTR( "):" ), endl );
            varBind->dump();

            // check maximum bind count
            if ( i > SNMP_MAX_BIND_COUNT )
            {
               ERROR_1( FSTR( "max binds reached" ) );
               return false;
            }

            if ( !varBind->isOfType( TYPE_SEQUENCE ) )
            {
               ERROR_3( FSTR( "varBind(" ), i, FSTR( ") has no sequence" ) );
               return false;
            }

            if ( !varBind->hasCorrectOid() )
            {
               ERROR_3( FSTR( "varBind(" ), i, FSTR( ") has invalid oid type" ) );
               return false;
            }

            if ( !varBind->hasCorrectValue( pduHdr->isOfType( TYPE_SETREQ ) ) )
            {
               ERROR_3( FSTR( "varBind(" ), i, FSTR( ") has invalid value type" ) );
               return false;
            }

            // process OID
            uint8_t ret = bindProc( pduHdr->type, (ObjectIdentifier*) varBind->data,
                                    (Sequence*) out );

            if ( ret == 0 )
            {
               pduHdr->setErrorStatus( ERR_NO_SUCH_NAME );
               ret = -2;
            }
            out += ret;

            // update lengths
            len_delta += (int8_t) ( ret - varBind->length - 2 );

            varBind = pduBody->getVariableBinding( ++i );
         }

         // set response type
         pduHdr->type = TYPE_GETRESP;

         // update lengths
         hdr->length += len_delta;
         pduHdr->length += len_delta;
         pduHdr->getPduBody()->length += len_delta;

         pduHdr->dump();
         td->bytesTransferred = bytesTransferred + len_delta;
      }
      // send response here
   }
   return true;
}

bool Snmp::sendTrapMessage( TrapHeader& header )
{
   IpStackManager* stream = IpStackManager::instance();
   if ( !stream )
   {
      ERROR_1( FSTR( "No IpStackmanager available to send SNMP-Trap!" ) );
      return false;
   }
// if ( !stream->isLinkedUp() )
// {
// ERROR_1( FSTR("unable to send SNMP-Trap!") );
// return false;
// }

   DEBUG_H2( FSTR( "sendTrapMessage()" ), endl );

   PaketHeader paket;
   Integer version( V1 );
   paket.addObject( version );
   paket.addObject( *reinterpret_cast<String*>( communityString ) );
   paket.addObject( header );
   paket.dump();

   return stream->sendToUdp( MANAGER_PORT, &paket, paket.length + 2, remoteIp );
}

uint16_t Snmp::getNextOidNode( uint8_t*& data )
{
   uint16_t node = *data & 0x7F;

   if ( *data & 0x80 )
   {
      node <<= 7;
      node |= ( *( ++data ) & 0x7F );
   }
   if ( *data & 0x80 )
   {
      node <<= 2;
      node |= ( *( ++data ) & 0x3 );
   }
   data++;
   return node;
}

Snmp::Object* Snmp::Object::get( uint8_t idx )
{
   uint8_t i = 0;
   while ( i < length )
   {
      Object* obj = (Object*) &data[i];
      if ( !idx )
      {
         if ( ( i + 2 + obj->length ) <= length )
         {
            return obj;
         }
         return NULL;
      }
      idx--;
      i += ( obj->length + 2 );
   }
   return NULL;
}

void Snmp::Object::setValue( uint32_t value )
{
   if ( value )
   {
      length = 0;
      while ( value )
      {
         uint8_t tmp = ( (convert_u*) &value )->byte[3];
         value <<= 8;
         if ( !length && !tmp )
         {
            continue;
         }
         data[length++] = tmp;
      }
   }
   else
   {
      data[0] = 0;
      length = 1;
   }
}

uint32_t Snmp::Object::getValue32()
{
   convert_u value;
   value.dword = 0;
   uint8_t i = 4;
   uint8_t* src = data;
   while ( i-- )
   {
      if ( i < length )
      {
         value.byte[i] = *src++;
      }
   }
   return value.dword;
}

void Snmp::Object::addObject( Object& obj )
{
   if ( MAX_OBJ_SIZE <= (uint16_t) ( length + obj.length + 2 ) )
   {
      ERROR_1( FSTR( "can not add Object, not enough memory" ) )
      return;
   }
   if ( ( type != TYPE_SEQUENCE ) && ( type < TYPE_GETREQ ) )
   {
      ERROR_2( FSTR( "can not add Objects to " ), type );
      return;
   }
   memcpy( &data[length], &obj, obj.length + 2 );
   length += ( obj.length + 2 );
}

void Snmp::Object::dump()
{
   for ( uint8_t i = 0; i < ( length + 2 ); i++ )
   {
      DEBUG_L2( ( (uint8_t* )this )[i], ' ' );
   }
}

void Snmp::ObjectIdentifier::setNodes( uint16_t* oidArray, uint8_t size )
{
   type = Snmp::TYPE_OID;
   length = 0;

   if ( size < 2 )
   {
      return;
   }

   data[length++] = 40 * oidArray[0] + oidArray[1];

   uint8_t i = 2;
   while ( i < size )
   {
      if ( oidArray[i] >= 0x80 )
      {
         uint16_t node = ( oidArray[i] >> 7 );
         if ( node >= 0x80 )
         {
            data[length++] = ( ( node >> 7 ) | 0x80 );
         }
         data[length++] = ( ( node & 0x7F ) | 0x80 );
      }
      data[length++] = ( oidArray[i++] & 0x7F );
   }
}

uint8_t Snmp::ObjectIdentifier::getNodes( uint16_t* oidArray, uint8_t size )
{
   uint8_t idx = 0;
   if ( size > 0 )
   {
      oidArray[idx++] = data[0] / 40;
   }
   if ( size > 1 )
   {
      oidArray[idx++] = data[0] % 40;
   }

   uint8_t i = 1;
   while ( ( idx < size ) && ( i < length ) )
   {
      oidArray[idx] = 0;

      while ( i < length )
      {
         oidArray[idx] <<= 7;
         oidArray[idx] += ( data[i] & 0x7F );
         if ( ( data[i++] & 0x80 ) == 0 )
         {
            break;
         }
      }
      idx++;
   }
   return idx;
}
