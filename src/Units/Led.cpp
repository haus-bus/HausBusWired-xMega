/* 
* Led.cpp
*
* Created: 03.12.2016 04:05:53
* Author: haus-bus
*/


#include "Led.h"

uint8_t Led::ledCount = 0;
Led* Led::instances[LED_COUNT];

Led::Led( port_pin_t _portPin ) : portPin( _portPin )
{
	instances[ledCount] = this;
	setId( (ClassId::LED << 8 ) | ++ledCount );
	
} //Led

Led* Led::isLed( Object obj )
{
	if( obj.isClassId(ClassId::LED) && ( obj.getInstanceId() < ledCount ) )
	{
		return instances[obj.getInstanceId()];
	}
	return NULL;
}