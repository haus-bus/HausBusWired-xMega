/*
 * Enc28j60.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

// temporarly enable DEBUG output always
// #define _DEBUG_

#include "Enc28j60.h"
#include <DigitalOutput.h>
#include <FlashString.h>
#include <Protocols/Ethernet/MAC.h>

#include <util/delay.h>

#define getId() FSTR( "Enc28j60::" )

const uint8_t Enc28j60::debugLevel( DEBUG_LEVEL_OFF );

extern const uint8_t configurationData[] PROGMEM;
const uint8_t configurationData[] =
{
// enc registers

// tx buffer
   ENC_REG_ETXSTL, LBYTE( ENC_TX_BUFFER_START ), ENC_REG_ETXSTH, HBYTE(
      ENC_TX_BUFFER_START ), ENC_REG_ETXNDL, LBYTE( ENC_TX_BUFFER_END ),
   ENC_REG_ETXNDH, HBYTE( ENC_TX_BUFFER_END ),

   // rx buffer
   ENC_REG_ERXSTL, LBYTE( ENC_RX_BUFFER_START ), ENC_REG_ERXSTH, HBYTE(
      ENC_RX_BUFFER_START ), ENC_REG_ERXNDL, LBYTE( ENC_RX_BUFFER_END ),
   ENC_REG_ERXNDH, HBYTE( ENC_RX_BUFFER_END ),

   // push mac out of reset
   ENC_REG_MACON2, 0x00,

   // mac receive enable, rx and tx pause control frames enable
   ENC_REG_MACON1, ( 1 << ENC_BIT_MARXEN ) | ( 1 << ENC_BIT_RXPAUS )
   | ( 1 << ENC_BIT_TXPAUS ),

#ifdef FULL_DUPLEX
   // mac auto padding of small packets, add crc, frame length check, full duplex
   ENC_REG_MACON3, ( 1 << ENC_BIT_PADCFG0 ) | ( 1 << ENC_BIT_TXCRCEN ) | ( 1 << ENC_BIT_FRMLNEN ) | ( 1 << ENC_BIT_FULDPX ),
#else
   // mac auto padding of small packets, add crc, frame length check, half duplex
   ENC_REG_MACON3, ( 1 << ENC_BIT_PADCFG0 ) | ( 1 << ENC_BIT_TXCRCEN )
   | ( 1 << ENC_BIT_FRMLNEN ),

   ENC_REG_MACON4, ( 1 << ENC_BIT_DEFER ),
#endif

   // max framelength 1518
   ENC_REG_MAMXFLL, LBYTE( 1518 ), ENC_REG_MAMXFLH, HBYTE( 1518 ),

#ifdef FULL_DUPLEX
   // back-to-back inter packet gap delay time (0x15 for full duplex)
   ENC_REG_MABBIPG, 0x15,
#else
   // back-to-back inter packet gap delay time (0x12 for half duplex)
   ENC_REG_MABBIPG, 0x12,
#endif

   // non back-to-back inter packet gap delay time (should be 0x12)
   ENC_REG_MAIPGL, 0x12,

#ifndef FULL_DUPLEX
   // non back-to-back inter packet gap delay time high (should be 0x0C for half duplex)
   ENC_REG_MAIPGH, 0x0C,
#endif

   // our mac address
   // ENC_REG_MAADR5, MYMAC1, ENC_REG_MAADR4, MYMAC2, ENC_REG_MAADR3, MYMAC3, ENC_REG_MAADR2, MYMAC4, ENC_REG_MAADR1, MYMAC5, ENC_REG_MAADR0, MYMAC6,

   // disable CLKOUT pin
   ENC_REG_ECOCON, 0x00,

   // configure Pattern Match Filter to destination IP=0xFFFFFFFF and MagicNumber = 0xEFEF
   // ENC_REG_EPMM5, 0x0C,
   // ENC_REG_EPMCSH, 0x10,
   // ENC_REG_EPMCSL, 0x10,

   // enable Patern Match and Broadcast filters
   // ENC_REG_ERXFCON, 0x51,

   // end of enc registers marker
   0xFF, 0xFF,

   // now the phy registers (with 2 bytes data each)

#ifdef FULL_DUPLEX
   // set the PDPXMD full duplex mode bit on the phy
#define ENC_REG_PHCON1_VALUE ( 0x0000 | ( 1 << ENC_BIT_PDPXMD ) )
   ENC_REG_PHCON1, HBYTE( ENC_REG_PHCON1_VALUE ), LBYTE( ENC_REG_PHCON1_VALUE ),
#endif

#ifndef FULL_DUPLEX
   // in half duplex do not loop back transmitted data
#define ENC_REG_PHCON2_VALUE ( 0x0000 | ( 1 << ENC_BIT_HDLDIS ) )
   ENC_REG_PHCON2, HBYTE( ENC_REG_PHCON2_VALUE ), LBYTE(
      ENC_REG_PHCON2_VALUE ),
#endif

   // leds: leda (yellow) rx and tx activity, stretch to 40ms
   // ledb (green)  link status
#define ENC_REG_PHCON_VALUE ( 0x0000 | ( 1 << ENC_BIT_STRCH ) \
                              | ( 13 << ENC_BIT_LACFG0 )               \
                              | ( 3 << ENC_BIT_LBCFG0 ) )
   ENC_REG_PHLCON, HBYTE( ENC_REG_PHCON_VALUE ), LBYTE(
      ENC_REG_PHCON_VALUE ),

   // end of config data marker
   0xFF, 0xFF
};

Enc28j60::Enc28j60( Spi& _spi, DigitalOutput _chipSelectPin,
                    DigitalInput _interruptPin ) :
   spi( &_spi ), chipSelectPin( _chipSelectPin ), currentBank( 0 ),
   interruptPin( _interruptPin ), nextPacketPointer( 0 )
{

}

uint8_t Enc28j60::init()
{
   spi->init<true, SPI_MODE_0_gc, true, SPI_PRESCALER_DIV4_gc, false>();

   _delay_us( 10000 );
   DEBUG_H1( FSTR( "init..." ) );
   DEBUG_H4( FSTR( "interrupt Port: " ), interruptPin.getPortNumber(),
             FSTR( " Pin: " ), interruptPin.getPinNumber() );
   DEBUG_H4( FSTR( "chipSelect Port: " ), chipSelectPin.getPortNumber(),
             FSTR( " Pin: " ), chipSelectPin.getPinNumber() );

   reset();

   uint8_t retries = 10;
   // test if chip is connected
   while ( !isClockReady() )
   {
      if ( --retries == 0 )
      {
         DEBUG_M1( FSTR( "not connected" ) );
         return NOT_CONNECTED;
      }
      _delay_us( 1000 );
   }

   // get enc revision id
   DEBUG_M2( FSTR( "enc revid: 0x" ), readRegister( ENC_REG_EREVID ) );

   uint32_t address = FAR( configurationData );
   uint8_t r, d;

   // setup enc registers according to the enc_configdata struct
   while ( 1 )
   {
      r = Flash::read( address );
      d = Flash::read( address );
      if ( r == 0xFF && d == 0xFF )
      {
         break;
      }
      writeRegister( r, d );
   }
   // now the phy registers
   uint16_t u;
   while ( 1 )
   {
      r = Flash::read( address );
      d = Flash::read( address );
      if ( r == 0xFF && d == 0xFF )
      {
         break;
      }
      u = ( ( (uint16_t) d ) << 8 );
      d = Flash::read( address );
      u |= d;
      writePhyRegister( r, u );
   }

   // setup mac register
   writeRegister( ENC_REG_MAADR5, MAC::local.getAddress( 0 ) );
   writeRegister( ENC_REG_MAADR4, MAC::local.getAddress( 1 ) );
   writeRegister( ENC_REG_MAADR3, MAC::local.getAddress( 2 ) );
   writeRegister( ENC_REG_MAADR2, MAC::local.getAddress( 3 ) );
   writeRegister( ENC_REG_MAADR1, MAC::local.getAddress( 4 ) );
   writeRegister( ENC_REG_MAADR0, MAC::local.getAddress( 5 ) );

   // setup receive next packet pointer
   nextPacketPointer = ENC_RX_BUFFER_START;

   // configure the enc interrupt sources
   writeRegister(
      ENC_REG_EIE,
      ( 1 << ENC_BIT_INTIE ) | ( 1 << ENC_BIT_PKTIE ) | ( 0 << ENC_BIT_DMAIE )
      | ( 0 << ENC_BIT_LINKIE ) | ( 0 << ENC_BIT_TXIE ) | ( 0 << ENC_BIT_WOLIE )
      | ( 0 << ENC_BIT_TXERIE ) | ( 0 << ENC_BIT_RXERIE ) );

   // enable receive
   setRegisterBits( ENC_REG_ECON1, ( 1 << ENC_BIT_RXEN ) );

   // wait for LINK
   _delay_ms( 200 );

   return OK;
}

bool Enc28j60::isLinkedUp()
{
   return readPhyRegister( ENC_REG_PHSTAT1 ) & ( 1 << ENC_BIT_LLSTAT );
}

uint8_t Enc28j60::isNewPacketReceived()
{
   // check rx packet counter
   uint8_t packetcounter = readRegister( ENC_REG_EPKTCNT );
   if ( packetcounter )
   {
      DEBUG_H2( FSTR( "EPKTCNT=0x" ), packetcounter );
   }
   return packetcounter;
}

uint16_t Enc28j60::read( void* pData, uint16_t length )
{
   if ( !isInterruptPending() || !isNewPacketReceived() )
   {
      // no interrupt or packetcounter is 0, there is nothing to receive, go back
      return 0;
   }

   // set read pointer to next packet
   writeRegister( ENC_REG_ERDPTL, LBYTE( nextPacketPointer ) );
   writeRegister( ENC_REG_ERDPTH, HBYTE( nextPacketPointer ) );

   uint8_t rxheader[6];
   // read enc rx packet header
   readBuffer( rxheader, sizeof( rxheader ) );
   nextPacketPointer = rxheader[0];
   nextPacketPointer |= ( (uint16_t) ( rxheader[1] ) ) << 8;

   uint16_t len = rxheader[2];
   len |= ( (uint16_t) ( rxheader[3] ) ) << 8;

   uint16_t status = rxheader[4];
   status |= ( (uint16_t) ( rxheader[5] ) ) << 8;

   // added by Sjors: reset the ENC when needed
   // If the receive OK bit is not 1 or the zero bit is not zero, reset the enc chip and SPI
   if ( ( !( status & ( 1 << 7 ) ) ) || ( status & 0x8000 ) )
   {
      DEBUG_M2( FSTR( "status=0x" ), status );
      init();
      return 0;
   }

   // skip the checksum (4 bytes) at the end of the buffer
   len -= 4;

   // if the application buffer is to small, we just truncate
   if ( len > length )
   {
      len = length;
   }

   // now read the packet data into buffer
   uint8_t* buffer = reinterpret_cast<uint8_t*>( pData );
   readBuffer( buffer, len );

   // adjust the ERXRDPT pointer (= free this packet in rx buffer)
   if ( ( ( nextPacketPointer - 1 ) > ENC_RX_BUFFER_END )
      || ( ( nextPacketPointer - 1 ) < ENC_RX_BUFFER_START ) )
   {
      writeRegister( ENC_REG_ERXRDPTL, LBYTE( ENC_RX_BUFFER_END ) );
      writeRegister( ENC_REG_ERXRDPTH, HBYTE( ENC_RX_BUFFER_END ) );
   }
   else
   {
      if ( !( ( nextPacketPointer - 1 ) & 1 ) )
      {
         ERROR_1( FSTR( "ENC_REG_ERXRDPT is not odd" ) )
      }
      writeRegister( ENC_REG_ERXRDPTL, LBYTE( nextPacketPointer - 1 ) );
      writeRegister( ENC_REG_ERXRDPTH, HBYTE( nextPacketPointer - 1 ) );
   }

   // trigger a decrement of the rx packet counter
   // this will clear PKTIF if EPKTCNT reaches 0
   setRegisterBits( ENC_REG_ECON2, ( 1 << ENC_BIT_PKTDEC ) );

   // return number of bytes written to the buffer
   DEBUG_M2( FSTR( "received bytes: 0x" ), len );
   return len;
}

void Enc28j60::reset()
{
   select();
   uint8_t data = ENC_SPI_OP_SC;
   spi->write( data );
   deselect();

   // errata #2: wait for at least 300 us
   _delay_us( 2000 );
}

void Enc28j60::setUdpPort9Filter()
{
   // configure Pattern Match Filter to destination IP=0xFFFFFFFF and MagicNumber = 0xEFEF
   writeRegister( ENC_REG_EPMM5, 0x0C );
   writeRegister( ENC_REG_EPMCSH, 0x10 );
   writeRegister( ENC_REG_EPMCSL, 0x10 );

   // enable Patern Match and Broadcast filters
   writeRegister( ENC_REG_ERXFCON, 0x51 );
}

uint16_t Enc28j60::write( void* pData, uint16_t length )
{
   // setup write pointer
   writeRegister( ENC_REG_EWRPTL, LBYTE( ENC_TX_BUFFER_START ) );
   writeRegister( ENC_REG_EWRPTH, HBYTE( ENC_TX_BUFFER_START ) );

   // end pointer (points to last byte) to start + length
   writeRegister( ENC_REG_ETXNDL, LBYTE( ENC_TX_BUFFER_START + length ) );
   writeRegister( ENC_REG_ETXNDH, HBYTE( ENC_TX_BUFFER_START + length ) );

   // enc requires 1 control byte before the package
   uint8_t control = 0;
   writeBuffer( &control, 1 );

   // copy packet to enc buffer
   writeBuffer( static_cast<uint8_t*>( pData ), length );

   // errata B7 #13: late collision retries
   TransmitStatusVector tsv;
   uint8_t retries = 5;
   while ( --retries )
   {
      setRegisterBits( ENC_REG_ECON1, ( 1 << ENC_BIT_TXRST ) );
      clearRegisterBits( ENC_REG_ECON1, ( 1 << ENC_BIT_TXRST ) );

      // clear TXIF/TXERIF flags
      clearRegisterBits( ENC_REG_EIR, ( 1 << ENC_BIT_TXERIF ) );
      clearRegisterBits( ENC_REG_EIR, ( 1 << ENC_BIT_TXIF ) );

      // start transmission by setting the TXRTS bit
      setRegisterBits( ENC_REG_ECON1, ( 1 << ENC_BIT_TXRTS ) );

      DEBUG_H3( FSTR( "sending 0x" ), length, FSTR( " Bytes" ) );

      // wait up to 10 ms for the tx to finish
      uint8_t delay = 100;
      while ( delay-- )
      {
         if ( readRegister( ENC_REG_EIR )
              & ( ( 1 << ENC_BIT_TXIF ) | ( 1 << ENC_BIT_TXERIF ) ) )
         {
            break;
         }
         DEBUG_L1( '.' );
         _delay_us( 100 );
      }

      clearRegisterBits( ENC_REG_ECON1, ( 1 << ENC_BIT_TXRTS ) );

      uint8_t status = readRegister( ENC_REG_ESTAT );
      DEBUG_M2( FSTR( "status 0x" ), status );

      // set read pointer to vector
      uint16_t address = ENC_TX_BUFFER_START + length + 1;
      writeRegister( ENC_REG_ERDPTL, LBYTE( address ) );
      writeRegister( ENC_REG_ERDPTH, HBYTE( address ) );

      readBuffer( (uint8_t*) &tsv, sizeof( TransmitStatusVector ) );

      if ( status & ( 1 << ENC_BIT_TXABRT ) )
      {
         dumpTransmitStatusVector( tsv );
         ERROR_1( FSTR( "ENC TXABRT" ) );
         // TRX was aborted, check if retries are needed for late collision reason
         if ( ( status & ( 1 << ENC_BIT_LATECOL ) ) || tsv.lateCollision )
         {
            ERROR_1( FSTR( "late collision" ) );
            if ( retries )
            {
               continue;
            }
         }
      }
      break;
   }
   if ( ( length < 60 ) && tsv.bytesTransferred == 64 )
   {
      // all packets are automatically padded to 60 Bytes + 4Bytes CRC
      return length;
   }
   return tsv.bytesTransferred - 4; // 4 Byte CRC were added automatically, remove them from return value
}

void Enc28j60::powerDown()
{
//
//// disable receiver
// clearRegisterBits( ENC_REG_ECON1, (1 << ENC_BIT_RXEN) );
//
//// wait for current pending receive
// while ( readRegister( ENC_REG_ESTAT ) & ( 1 << ENC_BIT_RXBUSY ) )
// ;
//
//// wait for current pending transmit
// while ( readRegister( ENC_REG_ECON1 ) & ( 1 << ENC_BIT_TXRTS ) )
// ;
//
//// enter sleep mode
// setRegisterBits( ENC_REG_ECON2, (1 << ENC_BIT_VRPS) );
// setRegisterBits( ENC_REG_ECON2, (1 << ENC_BIT_PWRSV) );
}

void Enc28j60::powerUp()
{
//// leave power save mode
// clearRegisterBits( ENC_REG_ECON2, (1 << ENC_BIT_PWRSV) );
// while ( !isClockReady() )
// ;
//
//// enable receiver
// setRegisterBits( ENC_REG_ECON1, (1 << ENC_BIT_RXEN) );
}

void Enc28j60::clearRegisterBits( uint8_t reg, uint8_t bits )
{
   // no automatic bank switching in this function !!!
   uint8_t cmd = ( reg & ENC_REG_ADDR_MASK ) | ENC_SPI_OP_BFC;

   select();
   spi->write( cmd );
   spi->write( bits );
   deselect();

}

void Enc28j60::dumpTransmitStatusVector( TransmitStatusVector& tsv )
{
   DEBUG_M2( FSTR( "bytesTransferred   " ), tsv.bytesTransferred );
   DEBUG_M2( FSTR( "bytesCollided      " ), tsv.bytesCollided );
   DEBUG_M2( FSTR( "crcError           " ), tsv.crcError );
   DEBUG_M2( FSTR( "lengthCheckError   " ), tsv.lengthCheckError );
   DEBUG_M2( FSTR( "lengthOutOfRange   " ), tsv.lengthOutOfRange );
   DEBUG_M2( FSTR( "done               " ), tsv.done );
   DEBUG_M2( FSTR( "multicast          " ), tsv.multicast );
   DEBUG_M2( FSTR( "broadcast          " ), tsv.broadcast );
   DEBUG_M2( FSTR( "packetDeferred     " ), tsv.packetDeferred );
   DEBUG_M2( FSTR( "excessiveDeferred  " ), tsv.excessiveDeferred );
   DEBUG_M2( FSTR( "excessiveCollision " ), tsv.excessiveCollision );
   DEBUG_M2( FSTR( "lateCollision      " ), tsv.lateCollision );
   DEBUG_M2( FSTR( "giant              " ), tsv.giant );
   DEBUG_M2( FSTR( "underrun           " ), tsv.underrun );
   DEBUG_M2( FSTR( "totalBytesTransfer " ), tsv.totalBytesTransferred );
   DEBUG_M2( FSTR( "controlFrame       " ), tsv.controlFrame );
   DEBUG_M2( FSTR( "pauseControlFrame  " ), tsv.pauseControlFrame );
   DEBUG_M2( FSTR( "backpressureApply  " ), tsv.backpressureApplied );
   DEBUG_M2( FSTR( "vlanFrame          " ), tsv.vlanFrame );
}

void Enc28j60::readBuffer( uint8_t* buffer, uint16_t length )
{
   select();
   uint8_t command = ENC_SPI_OP_RBM;
   spi->write( command );

   while ( length-- )
   {
      *buffer = 0;
      spi->write( *buffer++ );
   }
   deselect();
}

uint16_t Enc28j60::readPhyRegister( uint8_t reg )
{
   uint16_t value;

   writeRegister( ENC_REG_MIREGADR, reg );
   writeRegister( ENC_REG_MICMD, ( 1 << ENC_BIT_MIIRD ) );

   _delay_us( 10 );
   waitWhileBusy();

   writeRegister( ENC_REG_MICMD, 0x00 );
   value = ( ( (uint16_t) readRegister( ENC_REG_MIRDH ) ) << 8 );
   value |= ( (uint16_t) readRegister( ENC_REG_MIRDL ) );

   return value;
}

uint8_t Enc28j60::readRegister( uint8_t reg )
{
   uint8_t addr = reg & ENC_REG_ADDR_MASK;

   if ( addr < 0x1A )
   {
      selectRegisterBank( reg );
   }

   uint8_t data = addr | ENC_SPI_OP_RCR;
   select();
   spi->write( data );
   if ( reg & ENC_REG_WAIT_MASK )
   {
      // read dummy
      spi->read( data );
   }
   spi->read( data );
   deselect();
   return data;
}

void Enc28j60::selectRegisterBank( uint8_t reg )
{
   uint8_t bank = ( reg & ENC_REG_BANK_MASK ) >> ENC_REG_BANK_SHIFT;
   if ( bank != currentBank )
   {
      // need to switch bank first
      clearRegisterBits( ENC_REG_ECON1, 0x03 << ENC_BIT_BSEL0 );
      if ( bank )
      {
         setRegisterBits( ENC_REG_ECON1, bank << ENC_BIT_BSEL0 );
      }
      currentBank = bank;
   }
}

void Enc28j60::setRegisterBits( uint8_t reg, uint8_t bits )
{
   // no automatic bank switching in this function !!!
   uint8_t cmd = ( reg & ENC_REG_ADDR_MASK ) | ENC_SPI_OP_BFS;

   select();
   spi->write( cmd );
   spi->write( bits );
   deselect();
}

void Enc28j60::writeBuffer( uint8_t* buffer, uint16_t length )
{
   select();

   uint8_t command = ENC_SPI_OP_WBM;
   spi->write( command );
   spi->write( buffer, length );

   deselect();
}

void Enc28j60::writePhyRegister( uint8_t reg, uint16_t value )
{
   writeRegister( ENC_REG_MIREGADR, reg );
   writeRegister( ENC_REG_MIWRL, LBYTE( value ) );
   writeRegister( ENC_REG_MIWRH, HBYTE( value ) );

   _delay_us( 10 );
   waitWhileBusy();
}

void Enc28j60::writeRegister( uint8_t reg, uint8_t value )
{
   unsigned char addr = reg & ENC_REG_ADDR_MASK;

   if ( addr < 0x1A )
   {
      selectRegisterBank( reg );
   }
   addr |= ENC_SPI_OP_WCR;
   select();
   spi->write( addr );
   spi->write( value );
   deselect();
}
