/*
 * DaliDimmerHw.h
 *
 *  Created on: 06.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_HwUnitBoards_DaliDimmerHw_H
#define Electronics_HwUnitBoards_DaliDimmerHw_H


#include "HwUnitBoards.h"

#include <Dali.h>

#include <IDimmerHw.h>


class DaliDimmerHw : public IDimmerHw
{
   public:

      struct Status {
         uint8_t error : 1;
         uint8_t lampFailture : 1;
         uint8_t lampEffort : 1;
         uint8_t boundaryError : 1;
         uint8_t steppingFinished : 1;
         uint8_t resetState : 1;
         uint8_t addressMissing : 1;
         uint8_t powerFailture : 1;
      };

      ////    Constructors and destructors    ////

      DaliDimmerHw( uint8_t _address, Dali& _daliHw );

      ////    Operations    ////

      virtual uint8_t hasError();

      virtual uint16_t isOn();

      virtual void on( uint8_t value );

      virtual uint8_t setMode( uint8_t mode );

      ////    Additional operations    ////

      inline uint8_t getAddress() const
      {
         return address;
      }

      inline void setAddress( uint8_t p_address )
      {
         address = p_address;
      }

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

   private:

      inline Dali& getDaliHw() const
      {
         return daliHw;
      }

      inline void setDaliHw( Dali& p_daliHw )
      {
         daliHw = p_daliHw;
      }

      ////    Attributes    ////

   public:

      uint8_t address;

   protected:

      static const uint8_t debugLevel;

   private:

      Dali& daliHw;
};

#endif
