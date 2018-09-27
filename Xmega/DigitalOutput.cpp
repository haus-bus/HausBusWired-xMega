/*
 * DigitalOutput.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
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