/*
 * Button.h
 *
 * Created: 03.12.2016 00:36:01
 *  Author: haus-bus
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_


#include "gpio.h"

/*! \brief Returns true if the button is pressed.
 *
 * \param button_gpio BUTTON to test (BUTTONx_GPIO).
 *
 * \note The pins of the specified BUTTONSs are set to GPIO input mode.
 */
#define BUTTON_isPressed(button_gpio)     ioport_pin_is_low(button_gpio)


#endif /* BUTTON_H_ */