/*
 * WiFi.h
 *
 *  Created on: 17.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Electronics_IntegratedCircuits_WiFi_H
#define Electronics_IntegratedCircuits_WiFi_H

#include <DigitalOutput.h>
#include <Peripherals/Usart.h>
class DigitalOutput;

class FlashString;

class delay;


class WiFi
{
   public:

    #define MIN_FACTORY_RESET_TIME  6000 // [ms]
    #define MIN_RESET_TIME           100 // [ms]

      ////    Constructors and destructors    ////

      WiFi( Usart& _usart, DigitalOutput _resetPin );

      ////    Operations    ////

      uint8_t enterCommandMode( bool sync = true );

      void exitCommandMode();

      // Factory setting parameter values see the following list:
      // netmode                         0
      // wifi_conf                        Hi-Link_,wpa2_aes,12345678
      // dhcpc                             1
      // net_ip                            192.168.11.254,255.255.255.0,192.168.11.1
      // net_dns                         192.168.11.1,8.8.8.8
      // dhcpd                            1
      // dhcpd_ip                       192.168.16.100,192.168.16.200,255.255.255.0,192.168.16.1
      // dhcpd_dns                    192.168.16.1,8.8.8.8
      // dhcpd_time                   86400
      // remoteip                       192.168.11.245
      // remoteport                   8080
      // remotepro                     tcp
      // timeout                          0
      // mode                             none
      // uart                               115200,8,n,1
      // uartpacklen                  64
      // uartpacktimeout           10
      // IP address                    192.168.16.254
      // Wifi password              12345678
      // Web username/password admin/admin
      uint16_t resetToFactory( bool sync = true );

      ////    Additional operations    ////

      inline Usart& getUsart() const
      {
         return *usart;
      }

   private:

      inline DigitalOutput* getResetPin() const
      {
         return (DigitalOutput*) &resetPin;
      }

      ////    Attributes    ////

   public:

      Usart* usart;

   private:

      DigitalOutput resetPin;
};

#endif
