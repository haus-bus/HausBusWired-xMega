/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: Electronics
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Electronics/Electronics.h
*********************************************************************/

#ifndef Electronics_Electronics_H
#define Electronics_Electronics_H

#include <avr/io.h>
//## dependency delay
#include <util/delay.h>
//## operation getFckE()
#include <DefaultTypes.h>
//## dependency Flash
class Flash;

//## package Electronics
//## auto_generated
class Enc28j60;

//## auto_generated
class SdCard;

//## auto_generated
class WiFi;

//## type FCKE
enum FCKE {
    FCKE_V0_1 = 1,
    FCKE_V2_0 = 20,
    FCKE_V3_0 = 30,
    FCKE_V4_0 = 40,
    FCKE_V4_1 = 41,
    FCKE_V4_2 = 42,
    FCKE_V4_4 = 44,
    FCKE_V4_6 = 46,
};

//## operation getFckE()
uint8_t getFckE();

#endif
/*********************************************************************
	File Path	: AR8/release/Electronics/Electronics.h
*********************************************************************/
