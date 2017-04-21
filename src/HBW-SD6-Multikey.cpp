/*
 * HBW-SD6-Multikey.cpp
 *
 * Created: 03.12.2016 02:31:44
 *  Author: Viktor Pankraz
 */ 


#define HMW_DEVICETYPE 0xB1

#define HARDWARE_VERSION 0x01
#define FIRMWARE_VERSION 0x0100


extern "C"
{
	#include <asf.h>
}
#include "HBWired/HBWired.h"
#include "HBWired/CDCStream.h"
#include "HBWired/RS485Stream.h"
#include "HBWired/HBWSwitch.h"
#include "HBWired/HBWKey.h"

struct hbw_config {
	uint8_t  logging_time;     // 0x0001
	uint32_t central_address;  // 0x0002 - 0x0005
	// 6 LEDs erst mal als Switches
	hbw_config_switch switchcfg[6];  // 0x0006 - 0x0011
	// 6 Taster
	hbw_config_key keycfg[6];        // 0x0012 - 0x002D
} config;

HBWDevice* device = NULL;
HBWChannel* channels[12];

static usart_serial_options_t usart_options = 
{
	.baudrate = USART_SERIAL_BAUDRATE,
	.charlength = USART_SERIAL_CHAR_LENGTH,
	.paritytype = USART_SERIAL_PARITY,
	.stopbits = USART_SERIAL_STOP_BIT
};

void setup()
{
	// Authorize interrupts
	irq_initialize_vectors();
	cpu_irq_enable();
	
	// Initialize the sleep manager service
	sleepmgr_init();
	
	// Initialize the clock service
	sysclk_init();
	board_init();
	
	// Initialize system timer
	rtc_init();
	
	// Initialize RS485 interface
	usart_serial_init(USART_SERIAL, &usart_options);
	
	// Enable USB Stack Device
	udc_start ();
	
	static CDCStream debugStream;
	static RS485Stream rs485( USART_SERIAL );
	
	channels[0] = new HBWSwitch(LED1_GPIO,&(config.switchcfg[0]));
	channels[1] = new HBWSwitch(LED2_GPIO,&(config.switchcfg[1]));
	channels[2] = new HBWSwitch(LED3_GPIO,&(config.switchcfg[2]));
	channels[3] = new HBWSwitch(LED4_GPIO,&(config.switchcfg[3]));
	channels[4] = new HBWSwitch(LED5_GPIO,&(config.switchcfg[4]));
	channels[5] = new HBWSwitch(LED6_GPIO,&(config.switchcfg[5]));
	
	channels[ 6] = new HBWKey(BUTTON_S1_GPIO,&(config.keycfg[0]));
	channels[ 7] = new HBWKey(BUTTON_S2_GPIO,&(config.keycfg[1]));
	channels[ 8] = new HBWKey(BUTTON_S3_GPIO,&(config.keycfg[2]));
	channels[ 9] = new HBWKey(BUTTON_S4_GPIO,&(config.keycfg[3]));
	channels[10] = new HBWKey(BUTTON_S5_GPIO,&(config.keycfg[4]));
	channels[11] = new HBWKey(BUTTON_S6_GPIO,&(config.keycfg[5]));
	
	device = new HBWDevice(	HMW_DEVICETYPE, HARDWARE_VERSION, FIRMWARE_VERSION,
							&rs485,RS485_TXEN_GPIO,sizeof(config),&config,12,channels,&debugStream,
							NULL, NULL);
	hbwdebug(F("B: 2A\n"));
}

int main (void)
{
	setup();
	device->loop();
}

// following functions are realizations of basic Arduino functions
unsigned long millis(void)
{
	return rtc_get_time();
}

void pinMode( uint8_t pin, uint8_t mode)
{
	port_pin_flags_t flags = IOPORT_DIR_INPUT;
	if( mode == OUTPUT )
	{
		flags = IOPORT_DIR_OUTPUT;
	}
	ioport_configure_pin( pin, flags );
}

void digitalWrite(uint8_t pin, uint8_t state)
{
	if( state ) ioport_set_pin_high( pin );
	else ioport_set_pin_low( pin );
}

int digitalRead( uint8_t pin )
{
	return ioport_get_value( pin );
} 
  