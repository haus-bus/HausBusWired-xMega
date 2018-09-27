/*
 * I2c.h
 *
 *  Created on: 21.08.2015
 *      Author: viktor.pankraz
 */

#ifndef I2C_H_
#define I2C_H_

#include <DefaultTypes.h>
#include <DigitalOutput.h>

class PortPin;


class I2c
{

   enum State
   {
      IDLE,
      MASTER,
      SLAVE,
      BUSY
   };
   ////    Constructors and destructors    ////

   public:

      I2c( PortPin sclPin, PortPin sdaPin );

      ////    Operations    ////

   public:

      bool isIdle();

      uint16_t sendReceive( uint8_t* data, uint16_t sendLength,
                            uint16_t receiveLength );

   protected:

      inline bool isMaster()
      {
         return ( state == MASTER );
      }

      inline bool isSlave()
      {
         return ( state == SLAVE );
      }

   private:

      void start();

      void stop();

      void sendReceiveByte( uint8_t& data );

////    Attributes    ////

   public:

   protected:

      State state;

   private:

      DigitalOutput scl;

      DigitalOutput sda;

      static const uint8_t debugLevel;

////    Relations and components    ////

};

#endif /* I2C_H_ */
