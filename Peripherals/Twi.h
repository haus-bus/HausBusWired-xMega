/*
 * Twi.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Twi_H
#define Peripherals_Twi_H

#include "Peripherals/Peripherals.h"
#include <Basics.h>
#include <IStream.h>

class Twi
{
   public:

      class Master;

      class Slave;

      static const uint8_t DEFAULT_ADDRESS = 0x55;

      enum Status
      {
         STATUS_OK,
         STATUS_START,
         STATUS_STOP,
         ERR_IO_ERROR,
         ERR_FLUSHED,
         ERR_TIMEOUT,
         ERR_BAD_DATA,
         ERR_PROTOCOL,
         ERR_UNSUPPORTED_DEV,
         ERR_NO_MEMORY,
         ERR_INVALID_ARG,
         ERR_BAD_ADDRESS,
         ERR_BUSY,
         ERR_BAD_FORMAT
      };

      class Master
      {
         ////    Operations    ////

         public:

            inline void disable()
            {
               reg.CTRLA &= ~TWI_MASTER_ENABLE_bm;
               uint8_t tmp = reg.STATUS;
               reg.STATUS = tmp;
            }

            inline void disableInterrupt()
            {
               reg.CTRLA &= ~TWI_MASTER_INTLVL_gm;
            }

            inline void enable()
            {
               reg.CTRLA |= TWI_MASTER_ENABLE_bm;
            }

            inline void enableInterrupt( TWI_MASTER_INTLVL_t level
                                            = TWI_MASTER_INTLVL_LO_gc )
            {
               reg.CTRLA = ( reg.CTRLA & ~TWI_MASTER_INTLVL_gm ) | level;
            }

            inline TWI_MASTER_BUSSTATE_t getBusState()
            {
               return (TWI_MASTER_BUSSTATE_t) ( reg.STATUS & TWI_MASTER_BUSSTATE_gm );
            }

            uint8_t handleStatus( uint8_t& data, bool finish );

            inline uint8_t isEnabled()
            {
               return reg.CTRLA & TWI_MASTER_ENABLE_bm;
            }

            inline uint8_t isNewStatusAvailable()
            {
               return ( reg.STATUS & ( TWI_MASTER_RIF_bm | TWI_MASTER_WIF_bm ) );
            }

            inline uint8_t isSclHold()
            {
               return ( reg.STATUS & TWI_MASTER_CLKHOLD_bm );
            }

            inline uint8_t isBusBusy()
            {
               return ( getBusState() == TWI_MASTER_BUSSTATE_BUSY_gc );
            }

            inline uint8_t isBusIdle()
            {
               return ( getBusState() == TWI_MASTER_BUSSTATE_IDLE_gc );
            }

            inline uint8_t isBusOwner()
            {
               return ( getBusState() == TWI_MASTER_BUSSTATE_OWNER_gc );
            }

            inline uint8_t isBusUnknown()
            {
               return ( getBusState() == TWI_MASTER_BUSSTATE_UNKNOWN_gc );
            }

            inline void setTimeout( TWI_MASTER_TIMEOUT_t timeout )
            {
               uint8_t temp = reg.CTRLB & ~TWI_MASTER_TIMEOUT_gm;
               reg.CTRLB = temp | timeout;
            }

            inline void startTransfer( uint8_t address )
            {
               DEBUG_M2( "SLA", ( isInTransmitMode( address ) ? 'W' : 'R' ) );
               reg.ADDR = address;
            }

            inline void writeByte( uint8_t data )
            {
               reg.DATA = data;
               DEBUG_L4( 'D', '/', data, '\\' );
            }

            inline uint8_t readByte()
            {
               uint8_t data = reg.DATA;
               DEBUG_L4( 'D', '/', data, '\\' );
               return data;
            }

            inline bool hasError()
            {
               uint8_t const status = reg.STATUS
                                      & ( TWI_MASTER_ARBLOST_bm || TWI_MASTER_BUSERR_bm
                                        || TWI_MASTER_RXACK_bm );

               if ( status & TWI_MASTER_ARBLOST_bm )
               {
                  DEBUG_L1( "AE" )
               }
               else if ( status & TWI_MASTER_BUSERR_bm )
               {
                  DEBUG_L1( "BE" );
               }
               else if ( status & TWI_MASTER_RXACK_bm )
               {
                  DEBUG_L1( 'N' );
               }
               return status;
            }

            inline void stopWriteTransfer()
            {
               reg.CTRLC = TWI_MASTER_CMD_STOP_gc;
               DEBUG_L1( "WP" );
            }

            inline void stopReadTransfer()
            {
               reg.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
               DEBUG_L1( "RP" );
            }

            inline void waitNewStatusAvailable()
            {
               while ( !isNewStatusAvailable() )
               {
               }
            }

            uint16_t write( uint8_t address, void* pData, uint16_t length, bool stop = true );

            uint16_t read( uint8_t address, void* pData, uint16_t length, bool stop = true );

         protected:

            inline uint8_t isInReceiveMode( uint8_t address )
            {
               return ( address & 1 );
            }

            inline uint8_t isInTransmitMode( uint8_t address )
            {
               return !isInReceiveMode( address );
            }

            ////    Attributes    ////

         public:

            TWI_MASTER_t reg;
      };

      class Slave
      {
         ////    Operations    ////

         public:

            inline void clearErrors()
            {
               reg.STATUS = ( TWI_SLAVE_BUSERR_bm | TWI_SLAVE_COLL_bm );
            }

            inline void disable()
            {
               reg.CTRLA &= ~TWI_SLAVE_ENABLE_bm;
               uint8_t tmp = reg.STATUS;
               reg.STATUS = tmp;
            }

            inline void disableInterrupt()
            {
               reg.CTRLA &= ~TWI_SLAVE_INTLVL_gm;
            }

            inline void enable()
            {
               reg.CTRLA |= TWI_SLAVE_ENABLE_bm;
            }

            inline void enableInterrupt( TWI_SLAVE_INTLVL_t level
                                            = TWI_SLAVE_INTLVL_LO_gc )
            {
               reg.CTRLA = ( reg.CTRLA & ~TWI_SLAVE_INTLVL_gm ) | level;
            }

            inline uint8_t getAddress()
            {
               return ( reg.ADDR >> 1 );
            }

            inline Twi& getItsTwi()
            {
               return *(Twi*) ( ( (register8_t*) this ) - 8 );
            }

            uint8_t handleStatus( uint8_t& data, bool finish );

            inline void handleStop();

            inline uint8_t isEnabled()
            {
               return reg.CTRLA & TWI_SLAVE_ENABLE_bm;
            }

            inline uint8_t isNewStatusAvailable()
            {
               return ( reg.STATUS & ( TWI_SLAVE_DIF_bm | TWI_SLAVE_APIF_bm ) );
            }

            inline uint8_t isSclHold()
            {
               return ( reg.STATUS & TWI_SLAVE_CLKHOLD_bm );
            }

            uint16_t read( void* pData, uint16_t length );

            inline void setAddress( uint8_t address )
            {
               address <<= 1;
               address |= ( reg.ADDR & 1 );
               reg.ADDR = address;
            }

            inline void waitNewStatusAvailable()
            {
               while ( !isNewStatusAvailable() )
               {
               }
            }

            inline void waitTransferCompleted( IStream::TransferDescriptor& transferDescriptor )
            {
               // wait for address match
               waitNewStatusAvailable();

               while ( handleStatus(
                          transferDescriptor.pData[transferDescriptor.bytesTransferred],
                          !transferDescriptor.bytesRemaining ) < STATUS_STOP )
               {
                  transferDescriptor.bytesTransferred++;
                  transferDescriptor.bytesRemaining--;
                  waitNewStatusAvailable();
               }
            }

            ////    Attributes    ////

            TWI_SLAVE_t reg;
      };

      ////    Constructors and destructors    ////

   protected:

      inline Twi()
      {

      }

      ////    Operations    ////

   public:

      template<bool generalCall, uint32_t frequency,
               TWI_MASTER_INTLVL_t interruptLevelMaster,
               TWI_SLAVE_INTLVL_t interruptLevelSlave>
      inline void init()
      {
         // setup master
         master.reg.CTRLA = interruptLevelMaster | TWI_MASTER_RIEN_bm
                            | TWI_MASTER_WIEN_bm | TWI_MASTER_ENABLE_bm;

         uint16_t baud = F_CPU / 2 / frequency - 5;

         if ( baud > 255 )
         {
            baud = 255;
            ERROR_2( FSTR( "Twi::init() can not reach frequency, f=0x" ),
                     (uint32_t)( F_CPU / 460 ) );
         }
         master.reg.BAUD = baud;
         master.reg.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;

         // setup slave
         slave.reg.CTRLA = interruptLevelSlave | TWI_SLAVE_DIEN_bm
                           | TWI_SLAVE_APIEN_bm | TWI_SLAVE_ENABLE_bm;

         slave.reg.ADDR
            = generalCall ? ( DEFAULT_ADDRESS << 1 ) | 1 : ( DEFAULT_ADDRESS << 1 );
         slave.reg.STATUS = TWI_SLAVE_COLL_bm | TWI_SLAVE_BUSERR_bm
                            | TWI_SLAVE_APIF_bm | TWI_SLAVE_DIF_bm;
      }

      template<uint8_t portNumber>
      inline static Twi& instance()
      {
         switch ( portNumber )
         {
#ifdef TWIC
            case PortC:
               return *reinterpret_cast<Twi*>( &TWIC );
#endif

#ifdef TWIE
            case PortE:
               return *reinterpret_cast<Twi*>( &TWIE );
#endif
         }

         fatalErrorLoop();
         return *(Twi*) 0;
      }

      inline static Twi& instance( uint8_t portNumber );

      inline uint8_t isBusBusy()
      {
         return master.isBusBusy();
      }

      inline uint8_t isBusIdle()
      {
         return master.isBusIdle();
      }

      inline uint8_t isBusOwner()
      {
         return master.isBusOwner();
      }

      inline uint8_t isBusUnknown()
      {
         return master.isBusUnknown();
      }

      ////    Attributes    ////

   protected:

      register8_t CTRL;
   public:

      Master master;

      Slave slave;

   private:

      static const uint8_t debugLevel;
};

inline Twi& Twi::instance( uint8_t portNumber )
{
   switch ( portNumber )
   {
#ifdef TWIC
      case PortC:
         return *reinterpret_cast<Twi*>( &TWIC );
#endif

#ifdef TWIE
      case PortE:
         return *reinterpret_cast<Twi*>( &TWIE );
#endif
   }

   fatalErrorLoop();
   return *(Twi*) 0;
}

inline void Twi::Slave::handleStop()
{
   // Disable stop interrupt
   reg.CTRLA &= ~TWI_SLAVE_PIEN_bm;

   register8_t* portIn = (
      ( this == (Twi::Slave*) &TWIC.SLAVE ) ? &PORTC.IN : &PORTE.IN );

   // Only clear the interrupt flag if within a "safe zone".
   while ( !getItsTwi().isBusIdle() && !isSclHold() )
   {
      // Ensure that the SCL line is low
      if ( !( *portIn & PIN1_bm ) )
      {
         if ( !( *portIn & PIN1_bm ) )
         {
            DEBUG_L1( '_' );
            break;
         }
      }
   }

   // Check for an pending address match interrupt
   if ( !isSclHold() )
   {
      // Clear APIF, according to flowchart don't ACK or NACK
      reg.STATUS |= TWI_SLAVE_APIF_bm;
      DEBUG_L1( '!' );
   }
}

#endif
