/*
 * AR8_Board.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_SystemBoards_AR8_AR8_Boards_AR8_Boards_H
#define Electronics_SystemBoards_AR8_AR8_Boards_AR8_Boards_H

#include <DefaultTypes.h>
#include <IrDecoder.h>
#include <TwiHw.h>
#include <Security/ModuleId.h>
#include "AR8.h"
#include <Peripherals/TimerCounter1.h>

class AR8SystemHw;

class Dali;

class DigitalOutput;

class DmaChannel;

class DmaController;

class Enc28j60;

class EventSystem;

class IoPort;

class PortPin;

class SlotHw;

static const uint8_t DALI_IN_PIN = Pin4;
static const uint8_t DALI_OUT_PIN = Pin5;

extern IrDecoder* irDecoder;

extern MOD_ID_SECTION const ModuleId moduleId;

static const uint16_t ZCD_DEFAULT_PERIOD( 40960 );

static const uint8_t debugLevel( DEBUG_LEVEL_OFF );

extern SlotHw slotHw[MAX_SLOTS];

extern Dali daliHw;

extern DigitalOutput chipSelectSdCard;

extern Enc28j60 enc28j60;

void configureInfraRedHw( PortPin portPin, IrDecoder* decoder );

#endif
