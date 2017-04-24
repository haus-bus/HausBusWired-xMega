/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	ioport_configure_pin(RS485_TXEN_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(RS485_RXEN_GPIO, IOPORT_MODE_INVERT_PIN | IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	
	ioport_configure_pin(STATUS_LED_GREEN_GPIO, IOPORT_MODE_INVERT_PIN | IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(STATUS_LED_RED_GPIO,   IOPORT_MODE_INVERT_PIN | IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	ioport_configure_pin(LED1_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED2_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED3_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED4_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED5_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED6_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);

	ioport_configure_pin(BUTTON_S1_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_S2_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_S3_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_S4_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_S5_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_S6_GPIO, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	
	#ifdef CONF_BOARD_ENABLE_USARTC0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT
	| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT);
	#endif

	#ifdef CONF_BOARD_ENABLE_USARTC1
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 7), IOPORT_DIR_OUTPUT
	| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 6), IOPORT_DIR_INPUT);
	#endif

	#ifdef CONF_BOARD_ENABLE_USARTD0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 3), IOPORT_DIR_OUTPUT
	| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 2), IOPORT_DIR_INPUT);
	#endif

	#ifdef CONF_BOARD_ENABLE_USARTD1
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 7), IOPORT_DIR_OUTPUT
	| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 6), IOPORT_DIR_INPUT);
	#endif

	#ifdef CONF_BOARD_ENABLE_USARTE0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 3), IOPORT_DIR_OUTPUT
	| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 2), IOPORT_DIR_INPUT);
	#endif
}
