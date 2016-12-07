/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

//! Number of LEDs.
#define LED_COUNT   6
#define PUSH_BUTTON_COUNT   6

//! Port to output LED 
#define LED_PORT_ID 2 //PORTC
#define LED_PORT_MASK 0x3F

//! Port to read buttons
#define PUSH_BUTTON_PORT_ID 0 // PortA
#define PUSH_BUTTON_PORT_MASK 0x3F


/*! \name GPIO Connections of LEDs
 * LEDS connector is connected to PORTC
 * \note
 * 
 */
//! @{
#  define STATUS_LED_GREEN_GPIO	 	IOPORT_CREATE_PIN(PORTR,0)
#  define STATUS_LED_RED_GPIO		IOPORT_CREATE_PIN(PORTR,1)
	
#  define LED1_GPIO   IOPORT_CREATE_PIN(PORTC,0)
#  define LED2_GPIO   IOPORT_CREATE_PIN(PORTC,1)
#  define LED3_GPIO   IOPORT_CREATE_PIN(PORTC,2)
#  define LED4_GPIO   IOPORT_CREATE_PIN(PORTC,3)
#  define LED5_GPIO   IOPORT_CREATE_PIN(PORTC,4)
#  define LED6_GPIO   IOPORT_CREATE_PIN(PORTC,5)
//! @}


/*! \name GPIO Connections of Push Buttons
 * SWITCHES connector is connected to PORTA.
 * \note
 * 
 */
//! @{
#  define BUTTON_S1_GPIO    IOPORT_CREATE_PIN(PORTA,0)
#  define BUTTON_S2_GPIO    IOPORT_CREATE_PIN(PORTA,1)
#  define BUTTON_S3_GPIO    IOPORT_CREATE_PIN(PORTA,2)
#  define BUTTON_S4_GPIO    IOPORT_CREATE_PIN(PORTA,3)
#  define BUTTON_S5_GPIO    IOPORT_CREATE_PIN(PORTA,4)
#  define BUTTON_S6_GPIO    IOPORT_CREATE_PIN(PORTA,5)
//! @}



#endif // USER_BOARD_H
