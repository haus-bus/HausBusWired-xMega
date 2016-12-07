/*
 * Led.h
 *
 * Created: 03.12.2016 00:36:57
 *  Author: haus-bus
 */ 


#ifndef LED_H_
#define LED_H_


#include "gpio.h"

/*! \brief Turns off the specified LEDs.
 *
 * \param led_gpio LED to turn off (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_off(led_gpio)     gpio_set_pin_high(led_gpio)

/*! \brief Turns on the specified LEDs.
 *
 * \param led_gpio LED to turn on (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_on(led_gpio)      gpio_set_pin_low(led_gpio)

/*! \brief Toggles the specified LEDs.
 *
 * \param led_gpio LED to toggle (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_toggle(led_gpio)  gpio_toggle_pin(led_gpio)



#endif /* LED_H_ */