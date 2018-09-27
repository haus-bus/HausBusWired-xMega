/*
 * Electronics.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_Electronics_H
#define Electronics_Electronics_H

#include <avr/io.h>
#include <util/delay.h>
#include <DefaultTypes.h>
class Flash;

class Enc28j60;

class SdCard;

class WiFi;

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

uint8_t getFckE();

#endif
