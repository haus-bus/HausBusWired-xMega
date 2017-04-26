/*
 * HBW-SD6-Multikey.cpp
 *
 * Created: 03.12.2016 02:31:44
 *  Author: Viktor Pankraz
 */ 


#define HMW_DEVICETYPE 0xB1

#define HARDWARE_VERSION 0x01
#define FIRMWARE_VERSION 0x0100


#include <Utils/SystemTime.h>
#include <Elements/SerialStream.h>

#include "HBWired/HBWired.h"
#include "HBWired/HBWKey.h"
#include "HBWired/HBWDimmer.h"
#include "HBWired/HBWDS1820.h"

struct hbw_config {
	uint8_t  logging_time;          // 0x0001
	uint32_t central_address;       // 0x0002 - 0x0005
	hbw_config_key keycfg[6];       // 0x0006 - 0x0011
	HBWDimmer::Config ledcfg[6];    // 0x0012 - 0x002D
    HBWDS1820::Config ds1820cfg;    // 0x002E - 0x002F
} config;

static HBWDevice* device = NULL;
static HBWChannel* channels[13];

static usart_serial_options_t dbg_options = 
{
	.baudrate = DBG_SERIAL_BAUDRATE,
	.charlength = DBG_SERIAL_CHAR_LENGTH,
	.paritytype = DBG_SERIAL_PARITY,
	.stopbits = DBG_SERIAL_STOP_BIT
};

static usart_serial_options_t rs485_options =
{
    .baudrate = RS485_SERIAL_BAUDRATE,
    .charlength = RS485_SERIAL_CHAR_LENGTH,
    .paritytype = RS485_SERIAL_PARITY,
    .stopbits = RS485_SERIAL_STOP_BIT
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
	
	// Initialize interfaces
	usart_serial_init(DBG_SERIAL, &dbg_options);
    usart_serial_init(RS485_SERIAL, &rs485_options);

    // Initialize system timer
    SystemTime::init();
	
	static SerialStream debugStream( DBG_SERIAL );
	static SerialStream rs485Stream( RS485_SERIAL );

    static HBWKey hbwKey1(BUTTON_S1_GPIO,&(config.keycfg[0]));
    static HBWKey hbwKey2(BUTTON_S2_GPIO,&(config.keycfg[1]));
    static HBWKey hbwKey3(BUTTON_S3_GPIO,&(config.keycfg[2]));
    static HBWKey hbwKey4(BUTTON_S4_GPIO,&(config.keycfg[3]));
    static HBWKey hbwKey5(BUTTON_S5_GPIO,&(config.keycfg[4]));
    static HBWKey hbwKey6(BUTTON_S6_GPIO,&(config.keycfg[5]));

    static PwmOutput led1( PWM_TCC0, PWM_CH_A, 5000 );
    static PwmOutput led2( PWM_TCC0, PWM_CH_B, 5000 );
    static PwmOutput led3( PWM_TCC0, PWM_CH_C, 5000 );
    static PwmOutput led4( PWM_TCC0, PWM_CH_D, 5000 );
    static PwmOutput led5( PWM_TCC1, PWM_CH_A, 5000 );
    static PwmOutput led6( PWM_TCC1, PWM_CH_B, 5000 );

    static HBWDimmer hbwLed1( &led1, &config.ledcfg[0] );
    static HBWDimmer hbwLed2( &led2, &config.ledcfg[1] );
    static HBWDimmer hbwLed3( &led3, &config.ledcfg[2] );
    static HBWDimmer hbwLed4( &led4, &config.ledcfg[3] );
    static HBWDimmer hbwLed5( &led5, &config.ledcfg[4] );
    static HBWDimmer hbwLed6( &led6, &config.ledcfg[5] );

    static HBWDS1820 hbwDs1820( OneWire( ONE_WIRE_GPIO ), &config.ds1820cfg );

    channels[ 0] = &hbwKey1;
    channels[ 1] = &hbwKey1;
    channels[ 2] = &hbwKey1;
    channels[ 3] = &hbwKey1;
    channels[ 4] = &hbwKey1;
    channels[ 5] = &hbwKey1;
    channels[ 6] = &hbwLed1;
    channels[ 7] = &hbwLed2;
    channels[ 8] = &hbwLed3;
    channels[ 9] = &hbwLed4;
    channels[10] = &hbwLed5;
    channels[11] = &hbwLed6;
    channels[12] = &hbwDs1820;
	
	device = new HBWDevice(	HMW_DEVICETYPE, HARDWARE_VERSION, FIRMWARE_VERSION,
							&rs485Stream,RS485_TXEN_GPIO,sizeof(config),&config,13,channels,&debugStream,
							NULL, NULL);
	hbwdebug(F("B: 2A\n"));
}

int main (void)
{
	setup();
	while( 1 )
	{
		device->loop();
	}
}

// following functions are realizations of some basic Arduino functions
unsigned long millis(void)
{
	return SystemTime::now();
}

void pinMode( uint8_t pin, uint8_t mode)
{
	if( mode == OUTPUT )
	{
		ioport_set_pin_dir( pin, IOPORT_DIR_OUTPUT );
	}
    else
    {
        ioport_set_pin_dir( pin, IOPORT_DIR_INPUT );
    }
}

void digitalWrite(uint8_t pin, uint8_t state)
{
	if( state ) 
    {
        ioport_set_pin_high( pin );
    }
	else 
    {
        ioport_set_pin_low( pin );
    }
}

int digitalRead( uint8_t pin )
{
	return ioport_get_pin_level( pin );
} 
  