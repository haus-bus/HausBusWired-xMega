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

enum FIRMWARE_IDS
{
   SIM = 0,
   AR8_ID,
   MS6_ID,
   SD6_ID,
   SD485_ID,
   MAX_FW_ID
};

//
enum FCKE {
   FCKE_V0_1 = 1,
   FCKE_V2_0 = 20,
   FCKE_V3_0 = 30,
   FCKE_V4_0 = 40,
   FCKE_V4_1 = 41,
   FCKE_V4_2 = 42,
   FCKE_V4_3 = 43,
   FCKE_V4_4 = 44,
   FCKE_V4_5 = 45,
   FCKE_V4_6 = 46,
};

enum BoardRevisions
{
   AR8_RS485 = FCKE_V4_0,

   SD485_IO12 = FCKE_V4_0,

   SD485_B1 = FCKE_V4_1,
   SD485_B2 = FCKE_V4_2,
   SD485_B4 = FCKE_V4_4,
   SD485_B6 = FCKE_V4_6,

   SD485_LC4_0V = FCKE_V4_3,
   SD485_LC4_1V = FCKE_V4_5,
};

uint8_t getFckE();

#endif
