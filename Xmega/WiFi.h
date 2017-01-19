/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: WiFi
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Electronics/IntegratedCircuits/WiFi.h
*********************************************************************/

#ifndef Electronics_IntegratedCircuits_WiFi_H
#define Electronics_IntegratedCircuits_WiFi_H

//## auto_generated
#include "Electronics.h"
//## attribute resetPin
#include <DigitalOutput.h>
//## attribute usart
#include <Peripherals/Usart.h>
//## operation WiFi(Usart,DigitalOutput)
class DigitalOutput;

//## dependency FlashString
class FlashString;

//## dependency delay
class delay;

//## package Electronics::IntegratedCircuits

//## class WiFi
class WiFi {
public :

    //#[ type Constants
    #define MIN_FACTORY_RESET_TIME  6000 // [ms]
    #define MIN_RESET_TIME           100 // [ms]
    //#]
    
    ////    Constructors and destructors    ////
    
    //## operation WiFi(Usart,DigitalOutput)
    WiFi(Usart& _usart, DigitalOutput _resetPin);
    
    ////    Operations    ////
    
    //## operation enterCommandMode(bool)
    uint8_t enterCommandMode(bool sync = true);
    
    //## operation exitCommandMode()
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
    //## operation resetToFactory(bool)
    uint16_t resetToFactory(bool sync = true);
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline Usart& getUsart() const {
        //#[ auto_generated
        return usart;
        //#]
    }

private :

    //## auto_generated
    inline DigitalOutput* getResetPin() const {
        //#[ auto_generated
        return (DigitalOutput*) &resetPin;
        //#]
    }
    
    ////    Attributes    ////

public :

    Usart& usart;		//## attribute usart

private :

    DigitalOutput resetPin;		//## attribute resetPin
};

#endif
/*********************************************************************
	File Path	: AR8/release/Electronics/IntegratedCircuits/WiFi.h
*********************************************************************/
