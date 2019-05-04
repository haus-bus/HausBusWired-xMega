/*
 * KnxHw.h
 *
 *  Created on: 24.06.2014
 *      Author: viktor.pankraz
 */

#ifndef HwProtocols_Knx_KnxHw_H
#define HwProtocols_Knx_KnxHw_H

#include <DigitalInput.h>
#include <DigitalOutput.h>
#include <IStream.h>

class KnxHw
{
   ////    Constructors and destructors    ////

   public:

      KnxHw( DigitalInput _digitalIn, DigitalOutput _digitalOut );

      ////    Operations    ////

      void readByte();

      IStream::Status write( uint8_t* pData, uint16_t length );

   private:

      IStream::Status writeByte( uint8_t data );

      ////    Additional operations    ////

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
