/*
 * Spi.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Spi_H
#define Peripherals_Spi_H

#include "Peripherals.h"
#include <DigitalInput.h>
#include <DigitalOutput.h>


class Spi
{
   public:

      ////    Constructors and destructors    ////

   protected:

      inline Spi()
      {
      }

      ////    Operations    ////

   public:

      inline void disable()
      {
         reg.CTRL &= ~SPI_ENABLE_bm;
      }

      inline void disableInterrupt()
      {
         reg.INTCTRL = SPI_INTLVL_OFF_gc;
      }

      inline void enable()
      {
         reg.CTRL |= SPI_ENABLE_bm;
      }

      inline void enableInterrupt( SPI_INTLVL_t level = SPI_INTLVL_LO_gc )
      {
         reg.INTCTRL = level;
      }

      template<bool master, SPI_MODE_t mode, bool doubleClock,
               SPI_PRESCALER_t prescaler, bool lsbFirst>
      inline void init()
      {
         reg.CTRL = ( SPI_ENABLE_bm | ( lsbFirst ? SPI_DORD_bm : 0 )
                      | ( master ? SPI_MASTER_bm : 0 ) | ( doubleClock ? SPI_CLK2X_bm : 0 ) | mode
                      | prescaler );
      }

      template<uint8_t portNumber>
      static inline void configPortPins()
      {
         DigitalOutputTmpl<portNumber, 5> mosi;
         DigitalInputTmpl<portNumber, 6> miso;
         DigitalOutputTmpl<portNumber, 7> clk;
      }

      static Spi& instance( uint8_t portNumber );

      inline bool isMaster()
      {
         return ( reg.CTRL & SPI_MASTER_bm );
      }

      inline bool isTransferCompleted()
      {
         return ( reg.STATUS & SPI_IF_bm );
      }

      inline bool read( uint8_t& data )
      {
         // send dummy byte
         data = 0xFF;
         return write( data );
      }

      uint16_t read( void* pData, uint16_t length );

      inline uint8_t readDataRegister()
      {
         return reg.DATA;
      }

      inline void waitUntilTransferCompleted()
      {
         while ( !isTransferCompleted() )
         {
         }
      }

      inline bool wasError()
      {
         return ( reg.STATUS & SPI_WRCOL_bm );
      }

      inline bool write( uint8_t& data )
      {
         reg.DATA = data;
         waitUntilTransferCompleted();
         data = reg.DATA;
         return wasError();
      }

      uint16_t write( const void* pData, uint16_t length );

      inline void writeDataRegister( uint8_t data )
      {
         reg.DATA = data;
      }

      ////    Attributes    ////

   protected:

      SPI_t reg;
};

#endif
