/*
 * Object.h
 *
 * Created: 03.12.2016 03:19:48
 *  Author: haus-bus
 */ 


#ifndef OBJECT_H_
#define OBJECT_H_

#include <stdint.h>

class Object
{
	public:

	class ClassId
	{
		public:

		//## type Ids
		enum Ids
		{
			SYSTEM,
			BUTTON,
			LED,
			PWM_LED,
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
	return (id >> 8);
}

inline uint8_t Object::getInstanceId() const
{
	return static_cast<uint8_t>( id & 0xFF );
}

inline bool Object::isClassId( uint8_t classId )
{
	return (id >> 8) == classId;
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
	*((uint8_t*) &id) = number;
}


#endif /* OBJECT_H_ */