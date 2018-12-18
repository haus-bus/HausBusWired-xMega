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

enum CommChannel
{
   COMM_NO,
   COMM_UDP,
   COMM_RS485,
   COMM_TWI
};

HACF::ControlFrame* BooterHw::message;

BooterHw::TransferBuffer BooterHw::transferBuffer;

CommChannel activeComm = COMM_NO;

#ifdef SUPPORT_TWI
Twi& twi( Twi::instance<PortE>( ) );
#endif

#ifdef SUPPORT_RS485
enum RS485ProtocolDefines
{
   FRAME_STARTBYTE = 0xFD,
   FRAME_STOPBYTE = 0xFE,
   ESCAPE_BYTE = 0xFC
};

#if ( CONTROLLER_ID == 4 )
DigitalOutputTmpl<PortR, 0> rs485TxEnable;
#else
DigitalOutputTmpl<PortA, 5> rs485TxEnable;
#endif
DigitalInputTmpl<PortE, 2> rx;
DigitalOutputTmpl<PortE, 3> tx;
Usart& rs485( Usart::instance<PortE, 0>( ) );
#endif

#ifdef SUPPORT_UDP
Enc28j60 enc28j60( Spi::instance( PortC ), DigitalOutput( PortD, 4 ), DigitalInput( PortD, 5 ) );
#endif

#ifdef _DEBUG_
void putc( char c )
{
   Usart::instance<DBG_PORT, DBG_CHANNEL>().write( c );
}
#endif

void BooterHw::configure()
{
#ifdef _DEBUG_
   // configure Logger
   Usart::instance<DBG_PORT, DBG_CHANNEL>().init<DBG_BAUDRATE>();
   Usart::configPortPins<DBG_PORT, DBG_CHANNEL>();
   Logger::instance().setStream( putc );
   Logger::instance() << newTraceLine << FSTR( "Logger configured" );
#endif

   if ( getFirmwareId() != CONTROLLER_ID )
   {
      HomeAutomationConfiguration::restoreFactoryConfiguration( CONTROLLER_ID, BOARD_FCKE );
   }
#ifndef _DEBUG_
   InterruptController::selectBootInterruptSection();
#endif

#ifdef SUPPORT_UDP
   PORTC.DIRSET = Pin4 | Pin5 | Pin7;
   PORTC.OUTSET = Pin4;                 // deselect sd card
   PORTC.PIN6CTRL = PORT_OPC_PULLUP_gc; // pullup for SPI-MISO
   PORTD.DIRSET = Pin4 | Pin7;
   PORTD.OUTSET = Pin4;                 // deselect enc28j60
   PORTD.PIN5CTRL = PORT_OPC_PULLUP_gc; // pullup for interrupt

   uint16_t deviceId = HomeAutomationConfiguration::instance().getDeviceId();
   MAC::local.set( 0xAE, 0xDE, 0x48, 0, HBYTE( deviceId ), LBYTE( deviceId ) );
   if ( enc28j60.init() == Enc28j60::OK )
   {
      enc28j60.enableInterrupt();
   }
   else
#endif

#ifdef SUPPORT_RS485
#if ( CONTROLLER_ID != 4 )
   DigitalOutputTmpl<PortA, 6> rxEnable;
#endif

   rs485.init<135, -2, USART_CMODE_ASYNCHRONOUS_gc, USART_PMODE_EVEN_gc>();
#endif

#ifdef SUPPORT_TWI
   // enable pullup for TWI
   PORTCFG.MPCMASK = Pin0 | Pin1 | Pin2;
   PORTE.PIN0CTRL = PORT_OPC_PULLUP_gc;

   // enable power to external pullups
   PORTE.DIRSET = Pin3;
   PORTE.OUTSET = Pin3;
   twi.init<true, 30000, TWI_MASTER_INTLVL_OFF_gc, TWI_SLAVE_INTLVL_OFF_gc>();
#endif

#if ( CONTROLLER_ID == 1 )
   // configure status leds
   DigitalOutputTmpl<PortR, 0> greenLed;
   DigitalOutputTmpl<PortR, 1> redLed;
   greenLed.set();
#endif
#if ( CONTROLLER_ID == 4 )
   DigitalOutputTmpl<PortA, 6> redLed;
#endif
}

HACF::ControlFrame* BooterHw::getMessage()
{
   WatchDog::reset();
   RealTimeCounter::setCount( 0 );
   message = 0;

   while ( !message && ( RealTimeCounter::getCount() < MESSAGE_TIMEOUT ) )
   {
#ifdef SUPPORT_UDP
      if ( readMessageFromUdp() )
      {
         activeComm = COMM_UDP;
      }
      else
#endif
#ifdef SUPPORT_RS485
      if ( readMessageFromRS485() )
      {
         activeComm = COMM_RS485;
      }
      else
#endif
#ifdef SUPPORT_TWI
      if ( readMessageFromTwi() )
      {
         #if defined( SUPPORT_RS485 ) || defined( SUPPORT_UDP )
         activeComm = COMM_TWI;
         #endif
      }
      else
#endif
      {
      }
   }
   return message;
}

void BooterHw::sendMessage()
{
   // prepare header for RS485 or TWI
   TwiHeader* header = reinterpret_cast<TwiHeader*>( transferBuffer.header );
   header->address = 0;
   header->lastDeviceId = HomeAutomationConfiguration::instance().getDeviceId();

   uint16_t length = sizeof( TwiHeader ) - sizeof( header->unused ) + transferBuffer.controlFrame.getLength();

   header->checksum = 0;
   header->checksum = Checksum::get( &header->address, length );

   #ifdef SUPPORT_UDP
   if ( ( activeComm == COMM_UDP ) || ( activeComm == COMM_NO ) )
   {
      writeMessageToUdp();
   }
   else
   #endif
   #ifdef SUPPORT_RS485
   if ( ( activeComm == COMM_RS485 ) || ( activeComm == COMM_NO ) )
   {
      Rs485Header* header = reinterpret_cast<Rs485Header*>( transferBuffer.header );
      uint16_t length = sizeof( Rs485Header ) - sizeof( header->unused ) + transferBuffer.controlFrame.getLength();

      header->checksum = 0;
      header->checksum = Checksum::get( &header->checksum, length );
      writeMessageToRS485( &header->checksum, length );
   }
   else
   #endif
   #ifdef SUPPORT_TWI
   #if defined( SUPPORT_RS485 ) || defined( SUPPORT_UDP )
   if ( ( activeComm == COMM_TWI ) || ( activeComm == COMM_NO ) )
   {
      twi.master.write( header->address, &header->checksum, length - 1 );
   }
   else
   #else
   twi.master.write( header->address, &header->checksum, length - 1 );
   if ( false )
   #endif
   #endif
   {
      ERROR_1( FSTR( "NO COMM INTERFACE AVAILABLE" ) );
   }
}

#ifdef SUPPORT_TWI
bool BooterHw::readMessageFromTwi()
{
   if ( twi.slave.isNewStatusAvailable() )
   {
      TwiHeader* header = reinterpret_cast<TwiHeader*>( transferBuffer.header );
      uint16_t length = twi.slave.read( &header->address, sizeof( transferBuffer ) - sizeof( header->unused ) );

      if ( !Checksum::hasError( &header->address, length )
         && transferBuffer.controlFrame.isCommand()
         && transferBuffer.controlFrame.isRelevantForComponent()
         && transferBuffer.controlFrame.isRelevantForObject( HACF::BOOTLOADER_ID ) )
      {
         message = &transferBuffer.controlFrame;
         return true;
      }
   }
   return false;
}
#endif

#ifdef SUPPORT_RS485
bool BooterHw::readMessageFromRS485()
{
   static Rs485Header* const header = reinterpret_cast<Rs485Header*>( transferBuffer.header );
   static uint8_t* const receiveBuffer = &header->checksum;
   static const uint8_t* receiveBufferEnd = transferBuffer.header + sizeof( transferBuffer );

   static uint16_t receiveBufferPosition = 0;
   static bool pendingEscape = false;

   if ( rs485.isReceiveCompleted() )
   {
      uint8_t data;
      uint8_t status = rs485.getStatus();
      if ( !rs485.read( data ) )
      {
         ERROR_4( FSTR( "UART::status: 0x" ), status, FSTR( " at pos: " ), receiveBufferPosition );
      }

      if ( data == FRAME_STARTBYTE )
      {
         receiveBufferPosition = 0;
         pendingEscape = false;
      }
      else if ( data == FRAME_STOPBYTE )
      {
         if ( !Checksum::hasError( &header->checksum, receiveBufferPosition )
            && transferBuffer.controlFrame.isCommand()
            && transferBuffer.controlFrame.isRelevantForComponent()
            && transferBuffer.controlFrame.isRelevantForObject( HACF::BOOTLOADER_ID ) )
         {
            message = &transferBuffer.controlFrame;
            return true;
         }
      }
      else if ( data == ESCAPE_BYTE )
      {
         pendingEscape = true;
      }
      else
      {
         // restore data first
         if ( pendingEscape )
         {
            data |= 0x80;
            pendingEscape = false;
         }
         receiveBuffer[receiveBufferPosition] = data;
         if ( ( receiveBuffer + receiveBufferPosition ) < receiveBufferEnd )
         {
            receiveBufferPosition++;
         }
         else
         {
            // notify buffer overrun
            receiveBufferPosition = 0;
            ERROR_1( FSTR( "TOO MANY DATA WITHOUT STOP_BYTE" ) );
         }
      }
   }
   return false;
}

void BooterHw::writeMessageToRS485( uint8_t* pData, uint16_t length )
{
#ifdef WAIT_BEFORE_SEND
   uint16_t waitCnt = 0;
   do
   {
      rx.isSet() ? waitCnt++ : waitCnt = 0;
   }
   while ( waitCnt < HACF::deviceId );
#endif
   rs485.disableReceiver();
   rs485TxEnable.set();
   rs485.write( FRAME_STARTBYTE );

   uint16_t transmitIdx = 0;
   while ( transmitIdx < length )
   {
      uint8_t data = pData[transmitIdx++];
      if ( ( data == FRAME_STARTBYTE ) || ( data == FRAME_STOPBYTE ) || ( data == ESCAPE_BYTE ) )
      {
         rs485.write( ESCAPE_BYTE );
         data &= 0x7F;
      }
      rs485.write( data );
   }
   rs485.write( FRAME_STOPBYTE );
   rs485.waitUntilTransferCompleted();

   rs485TxEnable.clear();
   rs485.enableReceiver();
}
#endif

#ifdef SUPPORT_UDP
bool BooterHw::readMessageFromUdp()
{
   if ( enc28j60.isInterruptPending() )
   {
      uint16_t bytesTransferred = enc28j60.read( &transferBuffer, sizeof( transferBuffer ) );
      if ( bytesTransferred )
      {
         LanHeader* header = reinterpret_cast<LanHeader*>( transferBuffer.header );
         if ( header->udpHeader.isIpDatagramm() && header->udpHeader.isProtocollUdp()
            && header->udpHeader.isDestinationPort( UDP_PORT ) )
         {
            if ( ( bytesTransferred == UDP_MIN_PACKET_SIZE )
               || ( bytesTransferred == ( sizeof( transferBuffer ) - sizeof( transferBuffer.buffer )
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
               if ( ( IP::local == IP::defaultIp ) && !sourceIp->isBroadcast() )
               {
                  IP::local.setAddress( ( sourceIp->getAddress() & 0x00FFFFFF ) | 0xFE000000 );
               }
            }
         }
      }
   }
   return message != NULL;
}

void BooterHw::writeMessageToUdp()
{
   static uint16_t idCounter = 0;
   static LanHeader* const header = reinterpret_cast<LanHeader*>( transferBuffer.header );

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