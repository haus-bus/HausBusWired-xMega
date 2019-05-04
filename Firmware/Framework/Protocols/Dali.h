/*
 * Dali.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef HwProtocols_Dali_H
#define HwProtocols_Dali_H

#include "Protocols.h"
#include <DigitalInput.h>
#include <DigitalOutput.h>
#include <IStream.h>

class Dali
{
   public:

      // DALI uses 1200 baud, this waits for half of one bit time
    #define DALI_HALF_BIT_USEC 416
    #define DALI_HALF_BIT_WAIT _delay_us( DALI_HALF_BIT_USEC )

      enum Commands {
         OFF = 0,
         TO_LIGHT,
         TO_DARK,
         STEP_TO_LIGHT,
         STEP_TO_DARK,
         CALL_MAX_VALUE,
         CALL_MIN_VALUE,
         TO_DARK_AND_OFF,
         TO_LIGHT_AND_ON,
         GOTO_SCENE_X = 16,
         RESET = 32,
         STORE_VALUE_IN_DTR,
         STORE_DTR_AS_MAX_VALUE = 42,
         STORE_DTR_AS_MIN_VALUE,
         STORE_DTR_AS_SYSTEM_ERROR,
         STORE_DTR_AS_POWER_SUPPLY_ON,
         STORE_DTR_AS_STEP_TIME,
         STORE_DTR_AS_STEP_SPEED,
         STORE_DTR_AS_SCENE_X = 64,
         REMOVE_FROM_SCENE_X = 80,
         ADD_TO_GROUP_X = 96,
         REMOVE_FROM_GROUP_X = 112,
         STORE_DTR_AS_SCHORT_ADDR = 128,
         GET_STATUS = 144,
         IS_DEVICE_OK,
         HAS_LAMP_ERROR,
         HAS_POWER,
         HAS_BOUNDARY_ERROR,
         HAS_RESET_STATE,
         IS_SHORT_ADDR_MISSING,
         GET_VERSION,
         GET_DTR_VALUE,
         GET_DEVICE_TYPE,
         GET_PHYSICAL_MIN_VALUE,
         HAS_SUPLLY_ERROR,
         GET_CURR_VALUE = 160,
         GET_MAX_VALUE,
         GET_MIN_VALUE,
         GET_POWER_ON_VALUE,
         GET_SYSTEM_ERROR_VALUE,
         GET_STEP_TIME_AND_SPEED,
         GET_SCENE_X_VALUE = 176,
         GET_GROUP_1_TO_8_BELONGING = 192,
         GET_GROUP_9_TO_16_BELONGING,
         GET_DIRECT_ADDR_BYTE2,
         GET_DIRECT_ADDR_BYTE1,
         GET_DIRECT_ADDR_BYTE0
      };

      ////    Constructors and destructors    ////

      Dali( DigitalInput _digitalIn, DigitalOutput _digitalOut );

      ////    Operations    ////

      bool isConnected();

      IStream::Status read( uint8_t* pData );

      IStream::Status write( uint8_t* pData );

      ////    Additional operations    ////

   private:

      inline DigitalInput* getDigitalIn() const
      {
         return (DigitalInput*) &digitalIn;
      }

      inline DigitalOutput* getDigitalOut() const
      {
         return (DigitalOutput*) &digitalOut;
      }

      ////    Attributes    ////

      DigitalInput digitalIn;

      DigitalOutput digitalOut;
};

#endif
