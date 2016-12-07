/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "Elements/Led.h"
#include "Elements/Button.h"


int main (void)
{
	// Authorize interrupts
	irq_initialize_vectors();
	cpu_irq_enable();
	
	// Initialize the sleep manager service
	sleepmgr_init();
	
	// Initialize the clock service
	sysclk_init();
	board_init();
	
	// Enable USB Stack Device
	udc_start();
	
	LED_on( STATUS_LED_GREEN_GPIO );
	
	while (true) 
	{
		if( BUTTON_isPressed( BUTTON_S1_GPIO) ) LED_on(LED1_GPIO); 
		else LED_off(LED1_GPIO); 
		
		if( BUTTON_isPressed( BUTTON_S2_GPIO) ) LED_on(LED2_GPIO);
		else LED_off(LED2_GPIO); 
		 
		if( BUTTON_isPressed( BUTTON_S3_GPIO) ) LED_on(LED3_GPIO); 
		else LED_off(LED3_GPIO); 
		
		if( BUTTON_isPressed( BUTTON_S4_GPIO) ) LED_on(LED4_GPIO); 
		else LED_off(LED4_GPIO); 
		
		if( BUTTON_isPressed( BUTTON_S5_GPIO) ) LED_on(LED5_GPIO); 
		else LED_off(LED5_GPIO); 
		
		if( BUTTON_isPressed( BUTTON_S6_GPIO) ) LED_on(LED6_GPIO); 
		else LED_off(LED6_GPIO); 
	}
}
