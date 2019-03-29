/*
 * Object.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Object_H
#define SwFramework_Object_H

#include "SwFramework.h"
#include "DefaultTypes.h"

class Object
{
   public:

      class ClassId
      {
         public:

            enum Ids
            {
               SYSTEM = 0,
               PC_SERVER,
               WEATHER,
               RTC_TIME,
               LOGGER_MEMORY,
               BATTERY_MANAGER,
               MODBUS_SERVER = 0x0E,
               DIGITAL_PORT,
               BUTTON,
               DIMMER,
               ROLLER_SHUTTER,
               DIGITAL_OUTPUT,
               LOGICAL_BUTTON,
               LED,
               TEMPERATURE = 0x20,
               IR_RECEIVER,
               HUMIDITY,
               COUNTER,
               ANALOG_INPUT,
               MONITORED_DIGITAL_INPUT,
               USD_METER,
               DALI_LINE = 0xA0,
               KNX_LINE,
               ETHERNET,
               SMTP_AGENT,
               WIFI,
               GATEWAY = 0xB0,
               DHCP = 0xE0,
               IP_STACK_MANAGER,

               LOCAL_CONDITION = 0xFE
            };
      };

      ////    Constructors and destructors    ////

      inline Object( uint16_t _id = 0 ) :
         id( _id )
      {

      }

      ////    Operations    ////

      inline uint8_t getClassId();

      inline uint8_t getInstanceId() const;

      inline bool isClassId( uint8_t classId );

      inline bool isMe( uint16_t _id ) const;

      inline bool operator ==( const Object& object ) const;

      inline void setInstanceId( uint8_t number );

      ////    Additional operations    ////

      inline uint16_t getId() const
      {
         return id;
      }

      inline void setId( uint16_t p_id )
      {
         id = p_id;
      }

      ////    Attributes    ////

      uint16_t id;
};

inline uint8_t Object::getClassId()
{
   return ( id >> 8 );
}

inline uint8_t Object::getInstanceId() const
{
   return static_cast<uint8_t>( id & 0xFF );
}

inline bool Object::isClassId( uint8_t classId )
{
   return ( id >> 8 ) == classId;
}

inline bool Object::isMe( uint16_t _id ) const
{
   return id == _id;
}

inline bool Object::operator ==( const Object& object ) const
{
   return object.id == id;
}

inline void Object::setInstanceId( uint8_t number )
{
   *( (uint8_t*) &id ) = number;
}

#endif
