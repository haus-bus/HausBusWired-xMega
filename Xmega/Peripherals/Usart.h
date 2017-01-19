/*
 * Usart.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Usart_H
#define Peripherals_Usart_H

#include "Peripherals/Peripherals.h"
#include <Stream.h>

class Usart
{
public:

  ////    Constructors and destructors    ////

protected:

  inline Usart()
  {

  }

  ////    Operations    ////

public:

  inline void disableDataRegisterEmptyInterrupt()
  {
    reg.CTRLA &= ~USART_DREINTLVL_gm;
  }

  inline void disableReceiveCompleteInterrupt()
  {
    reg.CTRLA &= ~USART_RXCINTLVL_gm;
  }

  inline void disableReceiver()
  {
    reg.CTRLB &= ~USART_RXEN_bm;
  }

  inline void disableTransmitCompleteInterrupt()
  {
    reg.CTRLA &= ~USART_TXCINTLVL_gm;
  }

  inline void disableTransmitter()
  {
    reg.CTRLB &= ~USART_TXEN_bm;
  }

  inline void enableDataRegisterEmptyInterrupt( USART_DREINTLVL_t level =
      USART_DREINTLVL_LO_gc )
  {
    reg.CTRLA = ((reg.CTRLA & ~USART_DREINTLVL_gm) | level);
  }

  inline void enableReceiveCompleteInterrupt( USART_RXCINTLVL_t level =
      USART_RXCINTLVL_LO_gc )
  {
    reg.CTRLA = ((reg.CTRLA & ~USART_RXCINTLVL_gm) | level);
  }

  inline void enableReceiver()
  {
    reg.CTRLB |= USART_RXEN_bm;
  }

  inline void enableTransmitCompleteInterrupt( USART_TXCINTLVL_t level =
      USART_TXCINTLVL_LO_gc )
  {
    reg.CTRLA = ((reg.CTRLA & ~USART_TXCINTLVL_gm) | level);
  }

  inline void enableTransmitter()
  {
    reg.CTRLB |= USART_TXEN_bm;
  }

  inline void enableMpcm()
  {
    reg.CTRLB |= USART_MPCM_bm;
  }

  inline void disableMpcm()
  {
    reg.CTRLB &= ~USART_MPCM_bm;
  }

  inline void setAdressFrame()
  {
    reg.CTRLB |= USART_TXB8_bm;
  }

  inline void clearAdressFrame()
  {
    reg.CTRLB &= ~USART_TXB8_bm;
  }

  inline bool isAddressFrame()
  {
    return (reg.STATUS & USART_RXB8_bm);
  }

  template<uint32_t baudrate, bool doubleClock, USART_CMODE_t mode,
      USART_PMODE_t parity, USART_CHSIZE_t characterSize, bool twoStopBits> inline bool init()
  {
    if ( !checkSettings( baudrate ) ) return true;

    setBaudrate( getPrescaler<baudrate, doubleClock>(), 0 );
    setFormat( characterSize, parity, twoStopBits );
    reg.CTRLB = ( USART_TXEN_bm | USART_RXEN_bm
        | (doubleClock ? USART_CLK2X_bm : 0));

    return false;
  }

  static Usart& instance( uint8_t portNumber, uint8_t channel );

  inline bool isDataRegisterEmpty()
  {
    return (reg.STATUS & USART_DREIF_bm) || !isTransmitterEnabled();
  }

  inline bool isReceiveCompleted()
  {
    return (reg.STATUS & USART_RXCIF_bm);
  }

  inline bool isReceiverEnabled()
  {
    return (reg.CTRLB & USART_RXEN_bm);
  }

  inline bool isTransferCompleted()
  {
    return (reg.STATUS & USART_TXCIF_bm) || !isTransmitterEnabled();
  }

  inline bool isTransmitterEnabled()
  {
    return (reg.CTRLB & USART_TXEN_bm);
  }

  inline bool read( uint8_t& data )
  {
    bool result;

    while ( !isReceiveCompleted() )
      ;
    result = !wasError();
    data = reg.DATA;
    return result;
  }

  Stream::Status read( void * pData, uint16_t length );

  inline void setBaudrate( uint16_t prescaler, uint8_t scaleFactor )
  {
    reg.BAUDCTRLA = (uint8_t) prescaler;
    reg.BAUDCTRLB = (scaleFactor << USART_BSCALE0_bp) | (prescaler >> 8);
  }

  inline void setFormat( USART_CHSIZE_t characterSize, USART_PMODE_t parityMode,
                         bool twoStopBits )
  {
    reg.CTRLC = (characterSize | parityMode
        | (twoStopBits ? USART_SBMODE_bm : 0));
  }

  inline void setMode( USART_CMODE_t mode )
  {
    reg.CTRLC = ((reg.CTRLC & ~USART_CMODE_gm) | mode);
  }

  inline void waitUntilDataRegisterEmpty()
  {
    while ( !isDataRegisterEmpty() )
      ;
  }

  inline void waitUntilReceiveCompleted()
  {
    while ( !isReceiveCompleted() )
      ;
  }

  inline void waitUntilTransferCompleted()
  {
    reg.STATUS |= USART_TXCIF_bm;    // clear flag
    while ( !isTransferCompleted() )
      ;
  }

  inline bool wasDataOverunError()
  {
    return (reg.STATUS & USART_BUFOVF_bm);
  }

  inline bool wasError()
  {
    return (reg.STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm));
  }

  inline bool wasFrameError()
  {
    return (reg.STATUS & USART_FERR_bm);
  }

  inline bool wasParityError()
  {
    return (reg.STATUS & USART_PERR_bm);
  }

  inline bool write( uint8_t data )
  {
    if ( !isTransmitterEnabled() ) return false;
    waitUntilDataRegisterEmpty();
    reg.DATA = data;
    return true;
  }

  Stream::Status write( void * pData, uint16_t length );

private:

  inline bool checkSettings( const uint32_t& baudrate )
  {
    if ( baudrate > 1000000 ) return false;

    return true;
  }

  template<uint32_t baudrate, bool doubleClock> inline uint16_t getPrescaler()
  {
    uint16_t prescaler;
    if ( doubleClock )
    {
      prescaler = (F_CPU / 4 / baudrate);
      if ( prescaler & 0x01 ) prescaler++;
    }
    else
    {
      prescaler = (F_CPU / 8 / baudrate);
      if ( prescaler & 0x01 ) prescaler++;
    }

    return prescaler / 2 - 1;
  }

  ////    Attributes    ////

protected:

  USART_t reg;
};

#endif
