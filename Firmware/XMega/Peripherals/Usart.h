/*
 * Usart.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Usart_H
#define Peripherals_Usart_H

#include "Peripherals.h"
#include <DigitalOutput.h>

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

      inline void enableDataRegisterEmptyInterrupt( USART_DREINTLVL_t level
                                                       = USART_DREINTLVL_LO_gc )
      {
         reg.CTRLA = ( ( reg.CTRLA & ~USART_DREINTLVL_gm ) | level );
      }

      inline void enableReceiveCompleteInterrupt( USART_RXCINTLVL_t level
                                                     = USART_RXCINTLVL_LO_gc )
      {
         reg.CTRLA = ( ( reg.CTRLA & ~USART_RXCINTLVL_gm ) | level );
      }

      inline void enableReceiver()
      {
         reg.CTRLB |= USART_RXEN_bm;
      }

      inline void enableTransmitCompleteInterrupt( USART_TXCINTLVL_t level
                                                      = USART_TXCINTLVL_LO_gc )
      {
         reg.CTRLA = ( ( reg.CTRLA & ~USART_TXCINTLVL_gm ) | level );
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
         return ( reg.STATUS & USART_RXB8_bm );
      }

      template<uint32_t baudrate, USART_CMODE_t mode = USART_CMODE_ASYNCHRONOUS_gc,
               USART_PMODE_t parity = USART_PMODE_DISABLED_gc, USART_CHSIZE_t characterSize = USART_CHSIZE_8BIT_gc,
               bool twoStopBits = false, bool doubleClock = true>
      inline bool init()
      {
         const uint32_t minRate = doubleClock ? F_CPU / 128 / 8 / 4096 : F_CPU / 128 / 16 / 4096;
         const uint32_t maxRate = doubleClock ? F_CPU / 8 : F_CPU / 16;

         // check if baudrate can be achieved
         if ( ( baudrate < minRate ) || ( baudrate > maxRate ) )
         {
            return true;
         }

         setBaudrate( getPrescaler<baudrate, doubleClock>(), 0 );
         setFormat( mode, characterSize, parity, twoStopBits );
         reg.CTRLB = ( USART_TXEN_bm | USART_RXEN_bm | ( doubleClock ? USART_CLK2X_bm : 0 ) );

         return false;
      }

      template<uint16_t bsel, int8_t bscale, USART_CMODE_t mode = USART_CMODE_ASYNCHRONOUS_gc,
               USART_PMODE_t parity = USART_PMODE_DISABLED_gc, USART_CHSIZE_t characterSize = USART_CHSIZE_8BIT_gc,
               bool twoStopBits = false, bool doubleClock = false>
      inline bool init()
      {
         setBaudrate( bsel, bscale );
         setFormat( mode, characterSize, parity, twoStopBits );
         reg.CTRLB = ( USART_TXEN_bm | USART_RXEN_bm | ( doubleClock ? USART_CLK2X_bm : 0 ) );

         return false;
      }


      template<uint8_t portNumber, uint8_t channel>
      static inline void configPortPins()
      {
         if ( channel == 0 )
         {
            DigitalInputTmpl<portNumber, 2> rx;
            DigitalOutputTmpl<portNumber, 3> tx;
         }
         else if ( channel == 1 )
         {
            DigitalInputTmpl<portNumber, 6> rx;
            DigitalOutputTmpl<portNumber, 7> tx;
         }
      }

      template<uint8_t portNumber, uint8_t channel>
      static inline Usart& instance()
      {
         if ( channel == 0 )
         {
            switch ( portNumber )
            {
                                #ifdef USARTC0
               case PortC:
                  return *reinterpret_cast<Usart*>( &USARTC0 );
                                #endif

                                #ifdef USARTD0
               case PortD:
                  return *reinterpret_cast<Usart*>( &USARTD0 );
                                #endif

                                #ifdef USARTE0
               case PortE:
                  return *reinterpret_cast<Usart*>( &USARTE0 );
                                #endif

                                #ifdef USARTF0
               case PortF:
                  return *reinterpret_cast<Usart*>( &USARTF0 );
                                #endif
            }
         }
         else if ( channel == 1 )
         {
            switch ( portNumber )
            {
                                #ifdef USARTC1
               case PortC:
                  return *reinterpret_cast<Usart*>( &USARTC1 );
                                #endif

                                #ifdef USARTD1
               case PortD:
                  return *reinterpret_cast<Usart*>( &USARTD1 );
                                #endif

                                #ifdef USARTE1
               case PortE:
                  return *reinterpret_cast<Usart*>( &USARTE1 );
                                #endif

                                #ifdef USARTF1
               case PortF:
                  return *reinterpret_cast<Usart*> ( &USARTF1 );
                                #endif
            }
         }
         fatalErrorLoop();
         return *(Usart*) 0;
      }


      inline bool isDataRegisterEmpty()
      {
         return ( reg.STATUS & USART_DREIF_bm ) || !isTransmitterEnabled();
      }

      inline bool isReceiveCompleted()
      {
         return ( reg.STATUS & USART_RXCIF_bm );
      }

      inline bool isReceiverEnabled()
      {
         return ( reg.CTRLB & USART_RXEN_bm );
      }

      inline bool isTransferCompleted()
      {
         return ( reg.STATUS & USART_TXCIF_bm ) || !isTransmitterEnabled();
      }

      inline bool isTransmitterEnabled()
      {
         return ( reg.CTRLB & USART_TXEN_bm );
      }

      inline bool read( uint8_t& data )
      {
         bool result;

         while ( !isReceiveCompleted() )
         {
         }
         result = !wasError();
         data = reg.DATA;
         return result;
      }

      uint16_t read( void* pData, uint16_t length );

      inline uint8_t readDataRegisterFromISR()
      {
         return reg.DATA;
      }

      inline void setBaudrate( uint16_t prescaler, int8_t scaleFactor )
      {
         reg.BAUDCTRLB = ( scaleFactor << USART_BSCALE0_bp ) | ( ( prescaler >> 8 ) & 0x0F );
         reg.BAUDCTRLA = (uint8_t) prescaler;
      }

      inline void setFormat( USART_CMODE_t mode, USART_CHSIZE_t characterSize,
                             USART_PMODE_t parityMode, bool twoStopBits )
      {
         reg.CTRLC = ( mode | characterSize | parityMode
                       | ( twoStopBits ? USART_SBMODE_bm : 0 ) );
      }

      inline void setMode( USART_CMODE_t mode )
      {
         reg.CTRLC = ( ( reg.CTRLC & ~USART_CMODE_gm ) | mode );
      }

      inline void waitUntilDataRegisterEmpty()
      {
         while ( !isDataRegisterEmpty() )
         {
         }
      }

      inline void waitUntilReceiveCompleted()
      {
         while ( !isReceiveCompleted() )
         {
         }
      }

      inline void waitUntilTransferCompleted()
      {
         reg.STATUS |= USART_TXCIF_bm; // clear flag
         while ( !isTransferCompleted() )
         {
         }
      }

      inline bool wasDataOverunError()
      {
         return ( reg.STATUS & USART_BUFOVF_bm );
      }

      inline bool wasError()
      {
         return ( reg.STATUS & ( USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm ) );
      }

      inline bool wasFrameError()
      {
         return ( reg.STATUS & USART_FERR_bm );
      }

      inline bool wasParityError()
      {
         return ( reg.STATUS & USART_PERR_bm );
      }

      inline bool write( uint8_t data )
      {
         if ( !isTransmitterEnabled() )
         {
            return false;
         }
         waitUntilDataRegisterEmpty();
         reg.DATA = data;
         return true;
      }

      inline uint8_t getStatus() const
      {
         return reg.STATUS;
      }

      uint16_t write( void* pData, uint16_t length );

   private:

      inline bool checkSettings( const uint32_t& baudrate )
      {
         if ( baudrate > 1000000 )
         {
            return false;
         }

         return true;
      }

      template<uint32_t baudrate, bool doubleClock>
      inline uint16_t getPrescaler()
      {
         uint16_t prescaler;
         if ( doubleClock )
         {
            prescaler = ( F_CPU / 4 / baudrate );
            if ( prescaler & 0x01 )
            {
               prescaler++;
            }
         }
         else
         {
            prescaler = ( F_CPU / 8 / baudrate );
            if ( prescaler & 0x01 )
            {
               prescaler++;
            }
         }

         return prescaler / 2 - 1;
      }

      ////    Attributes    ////

   protected:

      USART_t reg;
};

#endif
