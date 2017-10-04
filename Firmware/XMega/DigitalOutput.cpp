/*
 * DigitalOutput.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#include "DigitalOutput.h"

void DigitalOutput::clear()
{
   getIoPort().clearPins( getPin() );
}

void DigitalOutput::set()
{
   getIoPort().setPins( getPin() );
}

void DigitalOutput::toggle()
{
   getIoPort().togglePins( getPin() );
}

