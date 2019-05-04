/*
 * HBCP.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_HomeAutomationControlFrame_HACF_H
#define SwFramework_Protocols_HomeAutomationControlFrame_HACF_H

#include <Tracing/Logger.h>

class HBCP
{
   public:

      class ControlFrame;

      class Header;

      class Object;

      static const uint8_t MAX_DATA_SIZE = 128;
      static const uint8_t MAX_BUFFER_SIZE = 32 + MAX_DATA_SIZE;
      static const uint8_t COMMANDS_START = 0;
      static const uint8_t COMMAND_WITH_RESPONSE_START = 0x40;
      static const uint8_t COMMANDS_END = 0x7F;
      static const uint8_t RESULTS_START = COMMANDS_END + 1;
      static const uint8_t RESULTS_END = 199;
      static const uint8_t EVENTS_START = RESULTS_END + 1;
      static const uint8_t EVENTS_END = 0xFF;
      static const uint8_t EVENT_ERROR = EVENTS_END;

      static const uint32_t BROADCAST_ID = 0;
      static const uint16_t SYSTEM_ID = 1;
      static const uint16_t BOOTLOADER_ID = 2;

      class Object
      {
         ////    Operations    ////

         public:
            Object()
            {
               address = 0;
               networkId = 0;
               classId = 0;
               instanceId = 0;
            }

            inline uint16_t getDeviceId() const;

            inline uint32_t getId() const;

            inline uint16_t getObjectId() const;

            inline void setDeviceId( uint16_t deviceId );

            inline void setId( uint32_t id );

            inline void setObjectId( uint16_t id );

            ////    Additional operations    ////

            inline uint8_t getInstanceId() const
            {
               return instanceId;
            }

            inline void setInstanceId( uint8_t p_instanceId )
            {
               instanceId = p_instanceId;
            }

            inline uint8_t getClassId() const
            {
               return classId;
            }

            inline void setClassId( uint8_t p_classId )
            {
               classId = p_classId;
            }

            inline uint8_t getAddress() const
            {
               return address;
            }

            inline void setAddress( uint8_t p_address )
            {
               address = p_address;
            }

            inline uint8_t getNetworkId() const
            {
               return networkId;
            }

            inline void setNetworkId( uint8_t p_networkId )
            {
               networkId = p_networkId;
            }

            ////    Attributes    ////

            uint8_t instanceId;

            uint8_t classId;

            uint8_t address;

            uint8_t networkId;
      } __attribute__( ( packed ) );

      class Header
      {
         ////    Additional operations    ////

         public:

            inline uint8_t getGatewayInstanceId() const
            {
               return gatewayInstanceId;
            }

            inline void setGatewayInstanceId( uint8_t p_gatewayInstanceId )
            {
               gatewayInstanceId = p_gatewayInstanceId;
            }

            inline uint8_t getReferenceCount() const
            {
               return referenceCount;
            }

            inline uint8_t decrementReferenceCount()
            {
               if ( referenceCount )
               {
                  referenceCount--;
                  DEBUG_M4( "sub ", (uintptr_t ) this, " ref ", referenceCount );
               }
               else
               {
                  ERROR_4( "err ", (uintptr_t ) this, " ref ", referenceCount );
               }
               return referenceCount;
            }

            inline uint8_t incrementReferenceCount()
            {
               if ( referenceCount < 0xFF )
               {
                  referenceCount++;
                  DEBUG_M4( "inc ", (uintptr_t ) this, " ref ", referenceCount );
               }
               else
               {
                  ERROR_4( "err ", (uintptr_t ) this, " ref ", referenceCount );
               }
               return referenceCount;
            }

            inline void resetReferenceCount()
            {
               referenceCount = 0;
            }

            inline void initReferenceCount()
            {
               referenceCount = 1;
               DEBUG_M4( "ini ", (uintptr_t ) this, " ref ", referenceCount );
            }

            inline uint16_t getSenderId() const
            {
               return senderId;
            }

            inline void setSenderId( uint16_t p_senderId )
            {
               senderId = p_senderId;
            }

            ////    Attributes    ////

         protected:

            uint8_t gatewayInstanceId;

            uint8_t referenceCount;

            uint16_t senderId;
      } __attribute__( ( packed ) );

      class ControlFrame
      {
         public:

            static const uint8_t DEFAULT_DATA_LENGTH = 8;
            static const uint32_t BROADCAST_ID = 0;

            ////    Operations    ////

            void encrypt();

            bool isValid();

            inline uint8_t* getData()
            {
               return &data[0];
            }

            uint16_t getLength() const;

            inline bool hasCorrectLength( uint16_t _length ) const
            {
               return ( _length == getLength() );
            }

            inline bool isBroadcast() const
            {
               return ( receiverId.networkId == BROADCAST_ID )
                      && ( receiverId.address == BROADCAST_ID );
            }

            inline uint8_t isCommand() const;

            inline uint8_t isCommand( uint8_t command ) const;

            inline uint8_t isCommandWithResponse() const;

            inline uint8_t isEvent() const;

            inline bool isForBootloader() const;

            inline bool isFromBootloader() const;

            inline bool isFromThisDevice() const
            {
               return senderId.getDeviceId() == deviceId;
            }

            inline bool isRelevantForComponent() const
            {
               return ( isBroadcast() || ( receiverId.getDeviceId() == deviceId ) );
            }

            inline bool isRelevantForObject( uint16_t id ) const;

            inline uint8_t isResult() const;

            inline uint8_t isResult( uint8_t result ) const;

            ////    Additional operations    ////

            inline uint8_t getControl() const
            {
               return control;
            }

            inline void setControl( uint8_t p_control )
            {
               control = p_control;
            }

            inline void setCommand( uint8_t p_command )
            {
               data[0] = p_command;
            }

            inline uint8_t getPacketCounter() const
            {
               return packetCounter;
            }

            inline void setPacketCounter( uint8_t p_packetCounter )
            {
               packetCounter = p_packetCounter;
            }

            inline Object* getSenderId() const
            {
               return (HBCP::Object*) &senderId;
            }

            inline Object* getReceiverId() const
            {
               return (HBCP::Object*) &receiverId;
            }

            inline uint16_t getDataLength() const
            {
               return dataLength;
            }

            inline void setDataLength( uint16_t p_dataLength )
            {
               dataLength = p_dataLength;
            }

            ////    Attributes    ////

            uint8_t control;

            uint8_t packetCounter;

            Object senderId;

            Object receiverId;

            uint16_t dataLength;

            uint8_t data[DEFAULT_DATA_LENGTH];
      } __attribute__( ( packed ) );

      ////    Constructors and destructors    ////

   protected:

      inline HBCP()
      {
      }

      ////    Operations    ////

   public:

      inline uint16_t getLength() const;

      inline bool hasCorrectLength( uint16_t length )
      {
         return ( length == getLength() );
      }

      inline bool isForThisDeviceOnly()
      {
         return ( controlFrame.receiverId.getDeviceId() == deviceId );
      }

      inline bool isFromThisDevice()
      {
         return controlFrame.isFromThisDevice();
      }

      void operator delete( void* obj, size_t size );

      HBCP* copy( void );

      ////    Additional operations    ////

      inline static uint16_t getDeviceId()
      {
         return deviceId;
      }

      inline static void setDeviceId( uint16_t p_deviceId )
      {
         deviceId = p_deviceId;
      }

      inline static uint16_t getReceiverIdForErrors()
      {
         return receiverIdForErrors;
      }

      inline static void setReceiverIdForErrors( uint16_t p_receiverIdForErrors )
      {
         receiverIdForErrors = p_receiverIdForErrors;
      }

      inline Header* getHeader() const
      {
         return (HBCP::Header*) &header;
      }

      inline ControlFrame* getControlFrame() const
      {
         return (HBCP::ControlFrame*) &controlFrame;
      }

      inline static uint16_t getMessagesInUse()
      {
         return messagesInUse;
      }

      inline static void setMessagesInUse( uint16_t p_messagesInUse )
      {
         messagesInUse = p_messagesInUse;
      }

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      ////    Attributes    ////

      static const uint8_t debugLevel;

   public:

      static uint16_t deviceId;

      static uint16_t receiverIdForErrors;

      static uint16_t messagesInUse;

      Header header;

      ControlFrame controlFrame;

} __attribute__( ( packed ) );

inline uint16_t HBCP::getLength() const
{
   return ( controlFrame.getLength() + sizeof( header ) );
}

inline uint16_t HBCP::Object::getDeviceId() const
{
   return ( networkId << 8 ) | address;
}

inline uint32_t HBCP::Object::getId() const
{
   convert_u convert;
   convert.byte[0] = instanceId;
   convert.byte[1] = classId;
   convert.byte[2] = address;
   convert.byte[3] = networkId;

   return convert.dword;
}

inline uint16_t HBCP::Object::getObjectId() const
{
   return (uint16_t) ( ( classId << 8 ) | instanceId );
}

inline void HBCP::Object::setDeviceId( uint16_t deviceId )
{
   networkId = HBYTE( deviceId );
   address = LBYTE( deviceId );
}

inline void HBCP::Object::setId( uint32_t id )
{
   setDeviceId( HWORD( id ) );
   setObjectId( LWORD( id ) );
}

inline void HBCP::Object::setObjectId( uint16_t id )
{
   classId = HBYTE( id );
   instanceId = LBYTE( id );
}

inline uint8_t HBCP::ControlFrame::isCommand() const
{
   return ( data[0] <= COMMANDS_END );
}

inline uint8_t HBCP::ControlFrame::isCommand( uint8_t command ) const
{
   return ( data[0] == command );
}

inline uint8_t HBCP::ControlFrame::isCommandWithResponse() const
{
   return isCommand() && ( data[0] & COMMAND_WITH_RESPONSE_START );
}

inline uint8_t HBCP::ControlFrame::isEvent() const
{
   return ( data[0] >= EVENTS_START );
}

inline bool HBCP::ControlFrame::isForBootloader() const
{
   return ( receiverId.getObjectId() == BOOTLOADER_ID );
}

inline bool HBCP::ControlFrame::isFromBootloader() const
{
   return ( senderId.getObjectId() == BOOTLOADER_ID );
}

inline bool HBCP::ControlFrame::isRelevantForObject( uint16_t id ) const
{
   if ( receiverId.classId == HBYTE( id ) )
   {
      return ( receiverId.instanceId == BROADCAST_ID )
             || ( LBYTE( id ) == receiverId.instanceId );
   }
   return false;
}

inline uint8_t HBCP::ControlFrame::isResult() const
{
   return !isEvent() && !isCommand();
}

inline uint8_t HBCP::ControlFrame::isResult( uint8_t result ) const
{
   return isResult() && ( data[0] == result );
}

#endif
