/*
 * Enc28j60.h
 *
 *  Created on: 24.06.2014
 *      Author: viktor.pankraz
 */

#ifndef Dummies_Enc28j60_H
#define Dummies_Enc28j60_H

#include "DefaultTypes.h"

class Enc28j60
{
   ////    Operations    ////

   public:

      bool isLinkedUp();

      uint16_t read( void* pData, uint16_t length );

      uint16_t write( void* pData, uint16_t length );

      void setUdpPort9Filter();
};

#endif
