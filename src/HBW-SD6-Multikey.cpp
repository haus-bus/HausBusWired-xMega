/*
 * HBW-SD6-Multikey.cpp
 *
 * Created: 03.12.2016 02:31:44
 *  Author: Viktor Pankraz
 */ 


#define HMW_DEVICETYPE 0xB1

#define HARDWARE_VERSION 0x01
#define FIRMWARE_VERSION 0x0103


#include <Utils/SystemTime.h>
#include <Elements/SerialStream.h>

#include "HBW-SD6-MultiKey.h"
#include "HBWired/HBWLinkKey.h"
#include "HBWired/HBWLinkDimmer.h"



struct hbw_config {
	uint8_t  loggingTime;           // 0x0001
	uint32_t centralAddress;        // 0x0002   - 0x0005
    uint16_t ledFeedback : 1;       // 0x0006:1
    uint16_t unused1 :15;           // 0x0006:2 - 0x0007
	HBWKey::Config keycfg[6];       // 0x0008   - 0x0013
	HBWDimmer::Config ledcfg[6];    // 0x0014   - 0x001F
    HBWDS1820::Config ds1820cfg;    // 0x0020   - 0x002F
    HBWAnalogIn::Config analogInCfg;// 0x0030   - 0x003F
    // HBWLinkKey                   // 0x0040   - 0x00F3
    // HBWLinkLed                   // 0x00F4   - 0x02E3
    //uint8_t reserved[0x3CB];
    //uint32_t ownAdress;
} config;

static HBWDevice* device = NULL;

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

void adc_init()
{
    struct adc_config adc_conf;
    struct adc_channel_config adcch_conf;

    adc_read_configuration(&ADC_BRIGHTNESS, &adc_conf);
    adcch_read_configuration(&ADC_BRIGHTNESS, ADC_BRIGHTNESS_CHANNEL, &adcch_conf);

    adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
    ADC_REF_BANDGAP);
    adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
    adc_set_clock_rate(&adc_conf, 200000UL);

    adcch_set_input(&adcch_conf, ADC_BRIGHTNESS_CH_POS, ADCCH_NEG_NONE, 1);

    adc_write_configuration(&ADC_BRIGHTNESS, &adc_conf);
    adcch_write_configuration(&ADC_BRIGHTNESS, ADC_BRIGHTNESS_CHANNEL, &adcch_conf);
}

void setup()
{
	// Initialize the sleep manager service
	sleepmgr_init();
	
	// Initialize the clock service
	sysclk_init();
	board_init();
	
	// Initialize interfaces
	usart_serial_init(DBG_SERIAL, &dbg_options);
    usart_serial_init(RS485_SERIAL, &rs485_options);

    // Initialize ADC to measure the brightness
    adc_init();

    // Initialize system timer
    SystemTime::init();

    // Authorize interrupts
    irq_initialize_vectors();
    cpu_irq_enable();

    static SerialStream debugStream( DBG_SERIAL );
    static SerialStream rs485Stream( RS485_SERIAL );
	
    static HBWKey hbwKey1(BUTTON_S1_GPIO,&(config.keycfg[0]) );
    static HBWKey hbwKey2(BUTTON_S2_GPIO,&(config.keycfg[1]) );
    static HBWKey hbwKey3(BUTTON_S3_GPIO,&(config.keycfg[2]) );
    static HBWKey hbwKey4(BUTTON_S4_GPIO,&(config.keycfg[3]) );
    static HBWKey hbwKey5(BUTTON_S5_GPIO,&(config.keycfg[4]) );
    static HBWKey hbwKey6(BUTTON_S6_GPIO,&(config.keycfg[5]) );

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
    static HBWAnalogIn hbwBrightness(&ADC_BRIGHTNESS, ADC_BRIGHTNESS_CHANNEL, &config.analogInCfg );

    hbwKey1.setFeedbackChannel( &hbwLed1 );
    hbwKey2.setFeedbackChannel( &hbwLed2 );
    hbwKey3.setFeedbackChannel( &hbwLed3 );
    hbwKey4.setFeedbackChannel( &hbwLed4 );
    hbwKey5.setFeedbackChannel( &hbwLed5 );
    hbwKey6.setFeedbackChannel( &hbwLed6 );

    static HBWLinkKey linkSender( 30, 0x0040 );
    static HBWLinkDimmer linkReceiver( 30, 0x00F4 );
	
	static HBWSD6Multikey sd6MultiKey( HMW_DEVICETYPE, HARDWARE_VERSION, FIRMWARE_VERSION,
							           &rs485Stream,RS485_TXEN_GPIO,sizeof(config),&config,&debugStream,
							           &linkSender, &linkReceiver);

    device = &sd6MultiKey;

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

void HBWSD6Multikey::afterReadConfig()
{
    // set global ledFeedback setting to each key object
    ::config.keycfg[0].ledFeedbackEnabled = ::config.ledFeedback;
    ::config.keycfg[1].ledFeedbackEnabled = ::config.ledFeedback;
    ::config.keycfg[2].ledFeedbackEnabled = ::config.ledFeedback;
    ::config.keycfg[3].ledFeedbackEnabled = ::config.ledFeedback;
    ::config.keycfg[4].ledFeedbackEnabled = ::config.ledFeedback;
    ::config.keycfg[5].ledFeedbackEnabled = ::config.ledFeedback;

    checkAndCorrectConfig( &::config.keycfg[0] );
    checkAndCorrectConfig( &::config.keycfg[1] );
    checkAndCorrectConfig( &::config.keycfg[2] );
    checkAndCorrectConfig( &::config.keycfg[3] );
    checkAndCorrectConfig( &::config.keycfg[4] );
    checkAndCorrectConfig( &::config.keycfg[5] );

    checkAndCorrectConfig( &::config.ledcfg[0] );
    checkAndCorrectConfig( &::config.ledcfg[1] );
    checkAndCorrectConfig( &::config.ledcfg[2] );
    checkAndCorrectConfig( &::config.ledcfg[3] );
    checkAndCorrectConfig( &::config.ledcfg[4] );
    checkAndCorrectConfig( &::config.ledcfg[5] );

    checkAndCorrectConfig( &::config.ds1820cfg );

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
  