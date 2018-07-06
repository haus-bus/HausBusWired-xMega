/*
 * DigitalOutputUnit.h
 *
 *  Created on: 03.07.2018
 *      Author: Viktor Pankraz
 */

#ifndef HwUnits_DigitalOutputUnit_H
#define HwUnits_DigitalOutputUnit_H

#include "HwUnits.h"
#include "PortPinUnit.h"

class Event;

class DigitalOutputUnit: public PortPinUnit
{
public:

  ////    Constructors and destructors    ////

  inline DigitalOutputUnit( PortPin _hardware ) : PortPinUnit( _hardware )
  {
	  
  };

  ////    Operations    ////

  virtual bool notifyEvent( const Event& event );

private:


  ////    Additional operations    ////

public:


protected:


private:


  ////    Attributes    ////

protected:

private:

};

#endif
