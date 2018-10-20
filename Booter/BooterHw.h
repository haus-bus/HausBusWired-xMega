/*
 * BooterHw.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef BooterHw_H
#define BooterHw_H

#include <Protocols/HACF.h>
#include <HomeAutomationHw.h>
#include <HomeAutomationConfiguration.h>
#include <Peripherals/RealTimeCounter.h>
#include <Peripherals/Twi.h>

#include <Enc28j60.h>
#include <Protocols/Ethernet/UdpHeader.h>


class Checksum;

class ModuleId;

class BooterHw : public HomeAutomationHw
{
   public:
      static const uint16_t UDP_MIN_PACKET_SIZE = 60;
      static const uint16_t MAGIC_NUMBER = 0xEFEF;
      static const uint32_t UDP_PORT = 9;
      static const uint16_t MESSAGE_TIMEOUT = 2 * 1024;

      static const uint8_t UDP_CHANNEL = 1;
      static const uint8_t USART_CHANNEL = 2;
      static const uint8_t TWI_CHANNEL = 4;

      struct LanHeader
      {
         UdpHeader udpHeader;
         uint16_t magicNumber;
      };

      struct TwiHeader
      {
         uint8_t unused[sizeof( LanHeader ) - 4];
         uint8_t address;
         uint8_t checksum;
         uint16_t lastDeviceId;
      };

      struct TransferBuffer
      {
         uint8_t header[sizeof( LanHeader )];
         HACF::ControlFrame controlFrame;
         uint8_t buffer[APP_SECTION_PAGE_SIZE];
      };

      ////    Constructors and destructors    ////

      inline BooterHw()
      {
      }

      ////    Operations    ////

      void configure();

      HACF::ControlFrame* getMessage();

      inline static uint16_t getNewDeviceId();

      void sendMessage();

      ////    Additional operations    ////

   private:

      bool readMessageFromTwi();

      bool readMessageFromUdp();

      void writeMessageToUdp();

      bool readMessageFromRS485();

      void writeMessageToRS485( uint8_t* pData, uint16_t length );

      ////    Attributes    ////

      static HACF::ControlFrame* message;

      static TransferBuffer transferBuffer;

   protected:

};

#endif
