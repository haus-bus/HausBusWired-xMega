/*
 * HwUnitBoards.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_HwUnitBoards_HwUnitBoards_H
#define Electronics_HwUnitBoards_HwUnitBoards_H

#include "Electronics.h"
class DaliDimmerHw;

class DigitalPortHw;

class RollerShutterHw;

class SlotHw;



// Turn off a power load generates a current spike. So wait some time to prevent errors in communication.
static const uint16_t POWER_SWITCH_DELAY = 6;

#endif
