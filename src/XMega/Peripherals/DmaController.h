/*
 * DmaController.h
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_DmaController_H
#define Peripherals_DmaController_H

#include "Peripherals.h"

class DmaController
{
   public:

      ////    Operations    ////

      inline static void disable()
      {
         DMA.CTRL = 0;
      }

      inline static void enable()
      {
         // Reset DMA controller just to make sure everything is from scratch
         DMA.CTRL = DMA_RESET_bm;
         DMA.CTRL = DMA_ENABLE_bm;
      }

      inline static uint8_t isBusy()
      {
         return DMA.STATUS & 0xF0;
      }

      inline static uint8_t isPending()
      {
         return DMA.STATUS & 0x0F;
      }

      inline static void reset()
      {
         DMA.CTRL &= ~DMA_ENABLE_bm;
         DMA.CTRL |= DMA_RESET_bm;
         DMA.CTRL &= ~DMA_RESET_bm;
      }

      inline static void setDoubleBufferMode( DMA_DBUFMODE_t mode )
      {
         DMA.CTRL = ( DMA.CTRL & ~DMA_PRIMODE_gm ) | mode;
      }

      inline static void setPrioMode( DMA_PRIMODE_t mode )
      {
         DMA.CTRL = ( DMA.CTRL & ~DMA_PRIMODE_gm ) | mode;
      }
};

#endif
