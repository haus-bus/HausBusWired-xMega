#ifndef Basics_InternalTemperature_H
#define Basics_InternalTemperature_H

#include "Basics.h"

class InternalTemperature
{
public:

  ////    Constructors and destructors    ////

  inline InternalTemperature()
  {
  }

  ////    Operations    ////

protected:

  ////    Additional operations    ////

public:

  static int16_t getValue();

  ////    Attributes    ////

protected:

  static const uint8_t debugLevel;

};

#endif

