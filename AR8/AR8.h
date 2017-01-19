/*
 * AR8.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef AR8_H
#define AR8_H

#include <SystemBoards.h>

static const uint8_t MAX_GATEWAYS        =    4;
static const uint8_t MAX_SENSORS         =   16;
static const uint8_t MAX_LOGICAL_BUTTONS =    8;
static const uint16_t SIZEOF_RULES       = 1600;

void notifyBusy();

void notifyIdle();

void putc( char c );

#endif
