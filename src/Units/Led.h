/* 
* Led.h
*
* Created: 03.12.2016 04:05:53
* Author: haus-bus
*/


#ifndef __LED_H__
#define __LED_H__

#include "Object.h"
#include <ioport.h>
#include <board.h>

class Led : public Object
{
//variables
public:
	static uint8_t ledCount;

protected:
	static Led* instances[LED_COUNT];

private:
	port_pin_t portPin;

//functions
public:
Led( port_pin_t _portPin );

static Led* isLed( Object obj );

	inline void on() { ioport_set_pin_high( portPin ); }
	inline void off() { ioport_set_pin_low( portPin ); }
	inline void toggle() { ioport_toggle_pin( portPin ); }

protected:
private:

}; //Led

#endif //__LED_H__
