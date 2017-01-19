/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: DmaChannel
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Peripherals/DmaChannel.cpp
*********************************************************************/

//## auto_generated
#include "Peripherals/DmaChannel.h"
//## dependency DmaController
#include "Peripherals/DmaController.h"
//## package Peripherals

//## class DmaChannel
DmaChannel& DmaChannel::instance(uint8_t number) {
    //#[ operation instance(uint8_t)
    return reinterpret_cast<DmaChannel*>(&DMA.CH0)[number];
    //#]
}

void DmaChannel::setupBlockTransfer(void * srcAddr, DMA_CH_SRCRELOAD_t srcReload, DMA_CH_SRCDIR_t srcDirection, void * destAddr, DMA_CH_DESTRELOAD_t destReload, DMA_CH_DESTDIR_t destDirection, uint16_t blockSize, DMA_CH_BURSTLEN_t burstMode, uint8_t repeatCount, bool useRepeat) {
    //#[ operation setupBlockTransfer(void *,DMA_CH_SRCRELOAD_t,DMA_CH_SRCDIR_t,void *,DMA_CH_DESTRELOAD_t,DMA_CH_DESTDIR_t,uint16_t,DMA_CH_BURSTLEN_t,uint8_t,bool)
    reg.SRCADDR0 = (( (uint16_t) srcAddr) >> 0*8 ) & 0xFF;
    reg.SRCADDR1 = (( (uint16_t) srcAddr) >> 1*8 ) & 0xFF;
    reg.SRCADDR2 = 0;
    
    reg.DESTADDR0 = (( (uint16_t) destAddr) >> 0*8 ) & 0xFF;
    reg.DESTADDR1 = (( (uint16_t) destAddr) >> 1*8 ) & 0xFF;
    reg.DESTADDR2 = 0;
    
    reg.ADDRCTRL = (uint8_t) srcReload | srcDirection | destReload | destDirection;
    reg.TRFCNT = blockSize;
    reg.CTRLA = ( reg.CTRLA & ~( DMA_CH_BURSTLEN_gm | DMA_CH_REPEAT_bm ) ) 
              | burstMode | ( useRepeat ? DMA_CH_REPEAT_bm : 0 );
    
    if ( useRepeat ) 
    {
    	reg.REPCNT = repeatCount;
    }
    //#]
}

uint8_t DmaChannel::getChannelMask() {
    //#[ operation getChannelMask()
    if( this == &instance(0) ) return DMA_CH0PEND_bm;
    if( this == &instance(1) ) return DMA_CH1PEND_bm;  
    if( this == &instance(2) ) return DMA_CH2PEND_bm;
    if( this == &instance(3) ) return DMA_CH3PEND_bm; 
    return 0;
    //#]
}

/*********************************************************************
	File Path	: Xmega192A3/debug/Peripherals/DmaChannel.cpp
*********************************************************************/
