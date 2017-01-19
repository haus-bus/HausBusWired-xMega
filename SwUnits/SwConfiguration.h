/*
 * SwConfiguration.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwConfiguration_H
#define SwConfiguration_H

#include "SwUnits.h"

#include <StorageInterface.h>

class Checksum;

class SwConfiguration
{
public:

  static const uint8_t NEW_INITIALIZED = 0x80;

  struct SmallConfiguration
  {
    uint16_t owner;
    uint8_t configData[5];
    uint8_t checksum;
  };

  struct LargeConfiguration
  {
    uint16_t owner;
    uint8_t configData[21];
    uint8_t checksum;
  };

  ////    Operations    ////

  static void updateChecksum( uint16_t offset, uint8_t size );

protected:

  static uint8_t getIndexOfConfiguration( uint16_t id, uint8_t size );

  ////    Attributes    ////

  static SmallConfiguration* smallConfigurations;

public:

  static StorageInterface* storage;

protected:

  static LargeConfiguration* largeConfigurations;

  static uint8_t maxSmallObjects;

  static uint8_t maxLargeObjects;

  static const uint8_t debugLevel;
};

#endif
