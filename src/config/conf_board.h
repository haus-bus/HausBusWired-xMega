/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H


//! Definition of USART connection for RS485
//! TXC0 on PE3 and RXC0 on PE2
#define  USART               USARTE0
#define  USART_RX_Vect       USARTE0_RXC_vect
#define  USART_DRE_Vect      USARTE0_DRE_vect
#define  USART_SYSCLK        SYSCLK_USART0
#define  USART_PORT          PORTE
#define  USART_PORT_PIN_TX   (1<<3)  // PE3 (TXC0)
#define  USART_PORT_PIN_RX   (1<<2)  // PE2 (RXC0)
#define  USART_PORT_SYSCLK   SYSCLK_PORT_E

#endif // CONF_BOARD_H
