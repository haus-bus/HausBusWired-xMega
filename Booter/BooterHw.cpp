/*
 * BooterHw.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "BooterHw.h"
#include <Security/Checksum.h>
#include <Security/Checksum16.h>
#include <Security/ModuleId.h>
#include <Release.h>

HACF::ControlFrame* BooterHw::message;

BooterHw::TransferBuffer BooterHw::transferBuffer;

Twi& BooterHw::twi( *reinterpret_cast<Twi*>( &TWIE ) );

#ifdef _DEBUG_
void putc( char c )
{
   Usart::instance( PortE, 0 ).write( c );
}
#endif

void BooterHw::configure()
{
#ifdef _DEBUG_
   // configure Logger
   Usart::instance( PortE, 0 ).init<BAUDRATE,   // baudrate
                                    true,       // doubleClock
                                    USART_CMODE_ASYNCHRONOUS_gc, // asynchronous communication
                                    USART_PMODE_DISABLED_gc, // NoParity
                                    USART_CHSIZE_8BIT_gc, // 8-Bits
                                    false       // 1 stopBit
                                    >();
   Logger::instance().setStream( putc );
#endif

   if ( getFirmwareId() != CONTROLLER_ID )
   {
      HwConfiguration::HomeAutomation::restoreFactoryConfiguration( CONTROLLER_ID, BOARD_FCKE );
   }
#ifndef _DEBUG_
   InterruptController::selectBootInterruptSection();
#endif

   // configure ports
   PORTR.DIRSET = Pin0 | Pin1;

   // enable pullup for TWI
   PORTCFG.MPCMASK = Pin0 | Pin1 | Pin2;
   PORTE.PIN0CTRL = PORT_OPC_PULLUP_gc;

   // enable power to external pullups
   PORTE.DIRSET = Pin3;
   PORTE.OUTSET = Pin3;

#ifdef SUPPORT_UDP
   PORTC.DIRSET = Pin4 | Pin5 | Pin7;
   PORTC.OUTSET = Pin4;                 // deselect sd card
   PORTC.PIN6CTRL = PORT_OPC_PULLUP_gc; // pullup for SPI-MISO
   PORTD.DIRSET = Pin4 | Pin7;
   PORTD.OUTSET = Pin4;                 // deselect enc28j60
   PORTD.PIN5CTRL = PORT_OPC_PULLUP_gc; // pullup for interrupt

   uint16_t deviceId = HwConfiguration::HomeAutomation::instance().getDeviceId();
   MAC::local = MAC( 0xAE, 0xDE, 0x48, 0, HBYTE( deviceId ), LBYTE( deviceId ) );
   udpAvailable = !enc28j60.init();
   if ( udpAvailable )
   {
      enc28j60.enableInterrupt();
   }
   else
#endif
   twi.init<true, 30000, TWI_MASTER_INTLVL_OFF_gc, TWI_SLAVE_INTLVL_OFF_gc>();

   // set statusLed to red
   PORTR.OUTSET = Pin0;
}

HACF::ControlFrame* BooterHw::getMessage()
{
   WatchDog::reset();
   RealTimeCounter::setCount( 0 );
   message = 0;

#ifdef SUPPORT_UDP
   if ( udpAvailable )
   {
      while ( !message && ( RealTimeCounter::getCount() < MESSAGE_TIMEOUT ) )
      {
         if ( enc28j60.isInterruptPending() )
         {
            readMessageFromUdp();
         }
      }
   }
   else
#endif
   {
      twi.slave.enable();
      while ( !message && ( RealTimeCounter::getCount() < MESSAGE_TIMEOUT ) )
      {
         if ( twi.slave.isNewStatusAvailable() )
         {
            readMessageFromTwi();
         }
      }
      twi.slave.disable();
   }
   return message;
}

void BooterHw::readMessageFromTwi()
{
   TwiHeader* header = reinterpret_cast<TwiHeader*>( transferBuffer.header );
   uint16_t length = twi.slave.read(
      &header->address, sizeof( transferBuffer ) - sizeof( header->unused ) );

   if ( !Checksum::hasError( &header->address, length )
      && transferBuffer.controlFrame.isCommand()
      && transferBuffer.controlFrame.isRelevantForComponent()
      && transferBuffer.controlFrame.isRelevantForObject( HACF::BOOTLOADER_ID ) )
   {
      message = &transferBuffer.controlFrame;
   }
}

void BooterHw::writeMessageToTwi()
{
   TwiHeader* header = reinterpret_cast<TwiHeader*>( transferBuffer.header );
   header->address = 0;
   header->lastDeviceId = HwConfiguration::HomeAutomation::instance().getDeviceId();

   uint16_t length = sizeof( TwiHeader ) - sizeof( header->unused )
                     + transferBuffer.controlFrame.getLength();

   header->checksum = 0;
   header->checksum = Checksum::get( &header->address, length );

   twi.master.write( header->address, &header->checksum, length-1 );
}

void BooterHw::sendMessage()
{
#ifdef SUPPORT_UDP
   if ( udpAvailable )
   {
      writeMessageToUdp();
   }
   else
#endif
   {
      writeMessageToTwi();
   }
}

#ifdef SUPPORT_UDP

bool BooterHw::udpAvailable;

uint16_t BooterHw::idCounter;

void BooterHw::readMessageFromUdp()
{
   uint16_t bytesTransferred = enc28j60.read( &transferBuffer,
                                              sizeof( transferBuffer ) );
   if ( bytesTransferred )
   {
      LanHeader* header = reinterpret_cast<LanHeader*>( transferBuffer.header );
      if ( header->udpHeader.isIpDatagramm() && header->udpHeader.isProtocollUdp()
         && header->udpHeader.isDestinationPort( UDP_PORT ) )
      {
         if ( ( bytesTransferred == UDP_MIN_PACKET_SIZE )
            || ( bytesTransferred
                 == ( sizeof( transferBuffer ) - sizeof( transferBuffer.buffer )
                      - sizeof( transferBuffer.controlFrame )
                      + transferBuffer.controlFrame.getLength() ) ) )
         {
            if ( transferBuffer.controlFrame.isCommand()
               && transferBuffer.controlFrame.isRelevantForComponent()
               && transferBuffer.controlFrame.isRelevantForObject( HACF::BOOTLOADER_ID ) )
            {
               DEBUG_L3( endl, "data received: 0x", bytesTransferred );
               message = &transferBuffer.controlFrame;
            }
            IP* sourceIp = header->udpHeader.getSourceAddress();
            if ( ( IP::local == DEFAULT_IP ) && !sourceIp->isBroadcast() )
            {
               IP::local.setAddress(
                  ( sourceIp->getAddress() & 0x00FFFFFF ) | 0xFE000000 );
            }
         }
      }
   }
}

void BooterHw::writeMessageToUdp()
{
   LanHeader* header = reinterpret_cast<LanHeader*>( transferBuffer.header );
   header->magicNumber = MAGIC_NUMBER;

   // ETHERNET HEADER
   header->udpHeader.packetDestination.setBroadcast();
   header->udpHeader.packetSource = MAC::local;

   header->udpHeader.setPacketType( EthernetHeader::IP_DATAGRAMM );

   uint16_t dataLength = transferBuffer.controlFrame.getLength() + sizeof( LanHeader::magicNumber );

   // IP HEADER
   header->udpHeader.IpHeader::sourceAddress = IP::local;
   header->udpHeader.IpHeader::destinationAddress.setLocalBroadcast();
   header->udpHeader.IpHeader::setPacketSize( sizeof( UdpHeader ) - sizeof( EthernetHeader ) + dataLength );
   header->udpHeader.IpHeader::setProtocol( IpHeader::UDP_PROTOCOL );
   header->udpHeader.IpHeader::setFragmentOffset( IpHeader::DO_NOT_FRAGMENT );
   header->udpHeader.IpHeader::setTimeToLive( 128 ); // max. hops
   header->udpHeader.IpHeader::setId( ++idCounter );
   header->udpHeader.IpHeader::setVersion( 4 );
   header->udpHeader.IpHeader::setLength( ( sizeof( IpHeader ) - sizeof( EthernetHeader ) ) >> 2 );
   header->udpHeader.IpHeader::setTypeOfService( 0 );
   header->udpHeader.IpHeader::setChecksum();

   // UDP HEADER
   header->udpHeader.setSourcePort( UDP_PORT );
   header->udpHeader.setDestinationPort( UDP_PORT );
   header->udpHeader.setLength( sizeof( UdpHeader ) - sizeof( IpHeader ) + dataLength );
   header->udpHeader.setChecksum();

   enc28j60.write( &transferBuffer, dataLength + sizeof( UdpHeader ) );
}
#endif
