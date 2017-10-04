/*
 * DmaChannel.cpp
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#include "DmaChannel.h"
#include "DmaController.h"

DmaChannel& DmaChannel::instance( uint8_t number )
{
   return reinterpret_cast<DmaChannel*>( &DMA.CH0 )[number];
}

void DmaChannel::setupBlockTransfer( void* srcAddr, DMA_CH_SRCRELOAD_t srcReload, DMA_CH_SRCDIR_t srcDirection, void* destAddr, DMA_CH_DESTRELOAD_t destReload, DMA_CH_DESTDIR_t destDirection, uint16_t blockSize, DMA_CH_BURSTLEN_t burstMode, uint8_t repeatCount, bool useRepeat )
{
   reg.SRCADDR0 = ( ( (uint16_t) srcAddr ) >> 0 * 8 ) & 0xFF;
   reg.SRCADDR1 = ( ( (uint16_t) srcAddr ) >> 1 * 8 ) & 0xFF;
   reg.SRCADDR2 = 0;

   reg.DESTADDR0 = ( ( (uint16_t) destAddr ) >> 0 * 8 ) & 0xFF;
   reg.DESTADDR1 = ( ( (uint16_t) destAddr ) >> 1 * 8 ) & 0xFF;
   reg.DESTADDR2 = 0;

   reg.ADDRCTRL = (uint8_t) srcReload | srcDirection | destReload | destDirection;
   reg.TRFCNT = blockSize;
   reg.CTRLA = ( reg.CTRLA & ~( DMA_CH_BURSTLEN_gm | DMA_CH_REPEAT_bm ) )
               | burstMode | ( useRepeat ? DMA_CH_REPEAT_bm : 0 );

   if ( useRepeat )
   {
      reg.REPCNT = repeatCount;
   }
}

uint8_t DmaChannel::getChannelMask()
{
   if ( this == &instance( 0 ) )
   {
      return DMA_CH0PEND_bm;
   }
   if ( this == &instance( 1 ) )
   {
      return DMA_CH1PEND_bm;
   }
   if ( this == &instance( 2 ) )
   {
      return DMA_CH2PEND_bm;
   }
   if ( this == &instance( 3 ) )
   {
      return DMA_CH3PEND_bm;
   }
   return 0;
}
