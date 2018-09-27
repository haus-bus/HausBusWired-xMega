/*
 * DmaChannel.h
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_DmaChannel_H
#define Peripherals_DmaChannel_H

#include "Peripherals.h"

class DmaController;

class DmaChannel
{
   public:

      ////    Operations    ////

      inline void disable()
      {
         reg.CTRLA &= ~DMA_CH_ENABLE_bm;
      }

      inline void disableInterrupt()
      {
         reg.CTRLB = ( reg.CTRLB & ~( DMA_CH_ERRINTLVL_gm | DMA_CH_TRNINTLVL_gm ) );
      }

      inline void disableSingleShot()
      {
         reg.CTRLA &= ~DMA_CH_SINGLE_bm;
      }

      inline void enable()
      {
         reg.CTRLA |= DMA_CH_ENABLE_bm;
      }

      inline void enableInterrupt( DMA_CH_TRNINTLVL_t transferLevel
                                      = DMA_CH_TRNINTLVL_LO_gc,
                                   DMA_CH_ERRINTLVL_t errorLevel
                                      = DMA_CH_ERRINTLVL_LO_gc )
      {
         reg.CTRLB = ( reg.CTRLB & ~( DMA_CH_ERRINTLVL_gm | DMA_CH_TRNINTLVL_gm ) )
                     | transferLevel | errorLevel;
      }

      inline void enableSingleShot()
      {
         reg.CTRLA |= DMA_CH_SINGLE_bm;
      }

      inline uint8_t getStatus()
      {
         return reg.CTRLB & ( DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm );
      }

      static DmaChannel& instance( uint8_t number );

      inline uint8_t isBusy()
      {
         return reg.CTRLB & DMA_CH_CHBUSY_bm;
      }

      inline uint8_t isEnabled()
      {
         return ( reg.CTRLA & DMA_CH_ENABLE_bm );
      }

      inline uint8_t isPending()
      {
         return reg.CTRLB & DMA_CH_CHPEND_bm;
      }

      inline void reset()
      {
         reg.CTRLA &= ~DMA_CH_ENABLE_bm;
         reg.CTRLA |= DMA_CH_RESET_bm;
         reg.CTRLA &= ~DMA_CH_RESET_bm;
      }

      inline void setTriggerSource( uint8_t trigger )
      {
         reg.TRIGSRC = trigger;
      }

      void setupBlockTransfer( void* srcAddr, DMA_CH_SRCRELOAD_t srcReload,
                               DMA_CH_SRCDIR_t srcDirection, void* destAddr,
                               DMA_CH_DESTRELOAD_t destReload,
                               DMA_CH_DESTDIR_t destDirection, uint16_t blockSize,
                               DMA_CH_BURSTLEN_t burstMode, uint8_t repeatCount,
                               bool useRepeat );

      inline void startTransfer()
      {
         reg.CTRLA |= DMA_CH_TRFREQ_bm;
      }

      inline uint8_t waitForNewStatus()
      {
         uint8_t status;
         do
         {
            status = getStatus();
         }
         while ( !status );

         reg.CTRLB = status;
         return status;
      }

   private:

      uint8_t getChannelMask();

      ////    Attributes    ////

   protected:

      DMA_CH_t reg;
};

#endif

